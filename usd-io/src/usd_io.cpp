// usd-io: narrow C API between MeshMaker's JS side and OpenUSD (wasm).
// Mesh data crosses the boundary as flat typed arrays; USD stages live and
// die inside single calls so no pxr types leak across the ABI.
//
// Export: builder pattern — begin, add N meshes (with MeshMaker's T*R*S item
// transforms authored as USD xformOps), end → usda text or usdc binary.
// Import: bytes of any format (usda/usdc/usd/usdz) go through a MEMFS temp
// file so SdfLayer's format detection does the work; every UsdGeomMesh is
// returned with its world transform baked into points (matching what the
// editor expects), Z-up stages converted to Y-up, and faces with more than
// four vertices fan-triangulated (Mesh2 holds only tris and quads).

#include <pxr/base/gf/matrix4d.h>
#include <pxr/base/gf/rotation.h>
#include <pxr/base/gf/range3f.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/base/plug/registry.h>
#include <pxr/base/tf/diagnosticMgr.h>
#include <pxr/base/tf/error.h>
#include <pxr/base/tf/stringUtils.h>
#include <pxr/base/vt/array.h>
#include <pxr/usd/ar/asset.h>
#include <pxr/usd/ar/resolver.h>
#include <pxr/usd/sdf/layerUtils.h>
#include <pxr/usd/sdf/layer.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/metrics.h>
#include <pxr/usd/usdGeom/tokens.h>
#include <pxr/usd/usdGeom/xform.h>
#include <pxr/usd/usdGeom/xformCache.h>
#include <pxr/usd/usdGeom/primvarsAPI.h>
#include <pxr/usd/usdShade/material.h>
#include <pxr/usd/usdShade/materialBindingAPI.h>
#include <pxr/usd/usdShade/shader.h>
#include <pxr/usd/usdUtils/usdzPackage.h>

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

using namespace pxr;

namespace {

std::string g_lastError;

// In wasm the default fatal path writes the reason only to a crash-log file
// it can never create, then exits — install a delegate so diagnostics reach
// stderr (and thus the JS console).
struct StderrDiagnosticDelegate : public TfDiagnosticMgr::Delegate {
    void IssueError(const TfError &err) override
    {
        g_lastError = err.GetCommentary();
        fprintf(stderr, "[usd error] %s\n", err.GetCommentary().c_str());
    }
    void IssueFatalError(const TfCallContext &ctx, const std::string &msg) override
    {
        fprintf(stderr, "[usd FATAL] %s (%s:%zu)\n", msg.c_str(),
                ctx.GetFile(), ctx.GetLine());
    }
    void IssueStatus(const TfStatus &status) override
    {
        fprintf(stderr, "[usd status] %s\n", status.GetCommentary().c_str());
    }
    void IssueWarning(const TfWarning &warning) override
    {
        fprintf(stderr, "[usd warning] %s\n", warning.GetCommentary().c_str());
    }
};

struct DelegateInstaller {
    StderrDiagnosticDelegate delegate;
    DelegateInstaller() { TfDiagnosticMgr::GetInstance().AddDelegate(&delegate); }
};
static DelegateInstaller g_delegateInstaller;

// The wasm build has no compiled-in plugin search path (PXR_INSTALL_LOCATION
// is unset and there is no shared-library path to anchor to), so point the
// registry at the resource tree the linker embedded at /usd.
void EnsurePluginsRegistered()
{
    static bool once = [] {
        PlugRegistry::GetInstance().RegisterPlugins(std::string("/usd/"));
        return true;
    }();
    (void)once;
}

struct Exporter {
    UsdStageRefPtr stage;
    int meshIndex = 0;
    std::vector<std::string> texturePaths;
};

struct ImportedMesh {
    std::string name;
    std::vector<float> points;    // xyz interleaved, world space, Y-up
    std::vector<int32_t> counts;  // faceVertexCounts, only 3s and 4s
    std::vector<int32_t> indices; // faceVertexIndices
    std::vector<float> uvs;       // face-varying uv pairs
    std::vector<uint8_t> texture; // encoded PNG/JPEG bytes
    float material[6] = {1, 1, 1, 1, 0, 0.4f}; // rgb, opacity, metallic, roughness
};

struct Scene {
    std::vector<ImportedMesh> meshes;
};

VtArray<GfVec3f> ComputeExtent(const VtArray<GfVec3f> &pts)
{
    GfRange3f range;
    for (const GfVec3f &p : pts)
        range.UnionWith(p);
    return {range.GetMin(), range.GetMax()};
}

} // namespace

extern "C" {

const char *usdio_last_error() { return g_lastError.c_str(); }

void *usdio_export_begin()
{
    EnsurePluginsRegistered();
    g_lastError.clear();

    auto *exp = new Exporter();
    exp->stage = UsdStage::CreateInMemory("meshmaker.usda");
    if (!exp->stage) {
        delete exp;
        return nullptr;
    }
    UsdGeomSetStageUpAxis(exp->stage, UsdGeomTokens->y);
    exp->stage->SetDefaultPrim(
        exp->stage->DefinePrim(SdfPath("/Root"), TfToken("Xform")));
    return exp;
}

// Transform components mirror Item::getTransformMatrix() = T * R * S, where
// R comes from glm::quat(eulerRadians) = Rx*Ry*Rz — Z applied first, which
// in USD is the rotateZYX op (value stays (xDeg, yDeg, zDeg)).
void usdio_export_add_mesh(void *h, const char *name,
                           const float *points, uint32_t numPoints,
                           const int32_t *faceVertexCounts, uint32_t numFaces,
                           const int32_t *faceVertexIndices, uint32_t numIndices,
                           const float *translate, const float *rotateDeg,
                           const float *scale,
                           const float *materialValues,
                           const float *uvs, uint32_t numUvs,
                           const uint8_t *textureBytes, uint32_t textureLen,
                           const char *textureExt)
{
    auto *exp = static_cast<Exporter *>(h);

    std::string base = TfMakeValidIdentifier(
        name && name[0] ? std::string(name) : "Mesh");
    SdfPath path(TfStringPrintf("/Root/%s_%d", base.c_str(), exp->meshIndex++));

    UsdGeomMesh mesh = UsdGeomMesh::Define(exp->stage, path);

    UsdGeomXformable xf(mesh);
    if (translate && (translate[0] || translate[1] || translate[2]))
        xf.AddTranslateOp().Set(
            GfVec3d(translate[0], translate[1], translate[2]));
    if (rotateDeg && (rotateDeg[0] || rotateDeg[1] || rotateDeg[2]))
        xf.AddRotateZYXOp().Set(
            GfVec3f(rotateDeg[0], rotateDeg[1], rotateDeg[2]));
    if (scale && (scale[0] != 1 || scale[1] != 1 || scale[2] != 1))
        xf.AddScaleOp().Set(GfVec3f(scale[0], scale[1], scale[2]));

    VtArray<GfVec3f> pts(numPoints);
    for (uint32_t i = 0; i < numPoints; ++i)
        pts[i] = GfVec3f(points[i * 3], points[i * 3 + 1], points[i * 3 + 2]);

    mesh.CreatePointsAttr().Set(pts);
    mesh.CreateExtentAttr().Set(ComputeExtent(pts));
    mesh.CreateFaceVertexCountsAttr().Set(
        VtArray<int32_t>(faceVertexCounts, faceVertexCounts + numFaces));
    mesh.CreateFaceVertexIndicesAttr().Set(
        VtArray<int32_t>(faceVertexIndices, faceVertexIndices + numIndices));
    // The mesh is the editing cage, not a subdiv limit surface.
    mesh.CreateSubdivisionSchemeAttr().Set(UsdGeomTokens->none);

    if (uvs && numUvs == numIndices) {
        VtArray<GfVec2f> st(numUvs);
        for (uint32_t i = 0; i < numUvs; ++i)
            st[i] = GfVec2f(uvs[i * 2], uvs[i * 2 + 1]);
        UsdGeomPrimvarsAPI(mesh).CreatePrimvar(
            TfToken("st"), SdfValueTypeNames->TexCoord2fArray,
            UsdGeomTokens->faceVarying).Set(st);
    }

    if (materialValues) {
        const SdfPath materialPath = path.AppendChild(TfToken("Material"));
        UsdShadeMaterial material = UsdShadeMaterial::Define(exp->stage, materialPath);
        UsdShadeShader surface = UsdShadeShader::Define(
            exp->stage, materialPath.AppendChild(TfToken("PreviewSurface")));
        surface.CreateIdAttr(VtValue(TfToken("UsdPreviewSurface")));
        surface.CreateInput(TfToken("diffuseColor"), SdfValueTypeNames->Color3f).Set(
            GfVec3f(materialValues[0], materialValues[1], materialValues[2]));
        surface.CreateInput(TfToken("opacity"), SdfValueTypeNames->Float).Set(materialValues[3]);
        surface.CreateInput(TfToken("metallic"), SdfValueTypeNames->Float).Set(materialValues[4]);
        surface.CreateInput(TfToken("roughness"), SdfValueTypeNames->Float).Set(materialValues[5]);
        surface.CreateOutput(TfToken("surface"), SdfValueTypeNames->Token);
        material.CreateSurfaceOutput().ConnectToSource(
            surface.ConnectableAPI(), TfToken("surface"));

        if (textureBytes && textureLen && uvs && numUvs == numIndices) {
        const std::string ext = textureExt && textureExt[0] ? textureExt : "png";
        const std::string fileName = TfStringPrintf("texture_%d.%s",
            exp->meshIndex - 1, ext.c_str());
        const std::string filePath = "/tmp/" + fileName;
        std::ofstream image(filePath, std::ios::binary | std::ios::trunc);
        image.write(reinterpret_cast<const char *>(textureBytes), textureLen);
        exp->texturePaths.push_back(filePath);

        UsdShadeShader reader = UsdShadeShader::Define(
            exp->stage, materialPath.AppendChild(TfToken("PrimvarReader")));
        reader.CreateIdAttr(VtValue(TfToken("UsdPrimvarReader_float2")));
        reader.CreateInput(TfToken("varname"), SdfValueTypeNames->Token).Set(TfToken("st"));
        reader.CreateOutput(TfToken("result"), SdfValueTypeNames->Float2);

        UsdShadeShader texture = UsdShadeShader::Define(
            exp->stage, materialPath.AppendChild(TfToken("DiffuseTexture")));
        texture.CreateIdAttr(VtValue(TfToken("UsdUVTexture")));
        texture.CreateInput(TfToken("file"), SdfValueTypeNames->Asset).Set(SdfAssetPath(fileName));
        texture.CreateInput(TfToken("st"), SdfValueTypeNames->Float2).ConnectToSource(
            reader.ConnectableAPI(), TfToken("result"));
        texture.CreateOutput(TfToken("rgb"), SdfValueTypeNames->Float3);
        surface.CreateInput(TfToken("diffuseColor"), SdfValueTypeNames->Color3f)
            .ConnectToSource(texture.ConnectableAPI(), TfToken("rgb"));
        }
        UsdShadeMaterialBindingAPI::Apply(mesh.GetPrim()).Bind(material);
    }
}

// format: 0 = usda text, 1 = usdc binary, 2 = usdz package.
// NUL-terminated but *outLen excludes the NUL). Frees the exporter either way.
uint8_t *usdio_export_end(void *h, int32_t format, uint32_t *outLen)
{
    std::unique_ptr<Exporter> exp(static_cast<Exporter *>(h));
    *outLen = 0;

    std::string data;
    if (format == 0) {
        if (!exp->stage->GetRootLayer()->ExportToString(&data))
            return nullptr;
    } else if (format == 1) {
        const char *tmp = "/tmp/usdio_export.usdc";
        if (!exp->stage->GetRootLayer()->Export(tmp))
            return nullptr;
        std::ifstream in(tmp, std::ios::binary);
        data.assign(std::istreambuf_iterator<char>(in),
                    std::istreambuf_iterator<char>());
        std::remove(tmp);
        if (data.empty())
            return nullptr;
    } else {
        const char *root = "/tmp/meshmaker.usdc";
        const char *package = "/tmp/usdio_export.usdz";
        if (!exp->stage->GetRootLayer()->Export(root) ||
            !UsdUtilsCreateNewUsdzPackage(SdfAssetPath(root), package, "meshmaker.usdc"))
            return nullptr;
        std::ifstream in(package, std::ios::binary);
        data.assign(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
        std::remove(root);
        std::remove(package);
    }

    for (const std::string &texturePath : exp->texturePaths)
        std::remove(texturePath.c_str());

    auto *out = static_cast<uint8_t *>(std::malloc(data.size() + 1));
    std::memcpy(out, data.data(), data.size());
    out[data.size()] = 0;
    *outLen = data.size();
    return out;
}

void usdio_free_buffer(uint8_t *p) { std::free(p); }

// Import any USD encoding. ext selects the temp file suffix ("usda", "usdc",
// "usd", "usdz") so SdfLayer's format detection applies. Returns an opaque
// Scene, or nullptr (see usdio_last_error).
void *usdio_import(const uint8_t *bytes, uint32_t len, const char *ext)
{
    EnsurePluginsRegistered();
    g_lastError.clear();

    std::string path = std::string("/tmp/usdio_import.") +
                       (ext && ext[0] ? ext : "usda");
    {
        std::ofstream out(path, std::ios::binary | std::ios::trunc);
        out.write(reinterpret_cast<const char *>(bytes), len);
        if (!out) {
            g_lastError = "failed to stage import bytes";
            return nullptr;
        }
    }

    UsdStageRefPtr stage = UsdStage::Open(path);
    if (!stage) {
        std::remove(path.c_str());
        if (g_lastError.empty())
            g_lastError = "not a readable USD file";
        return nullptr;
    }

    // Bake world transforms; convert Z-up stages to the editor's Y-up.
    GfMatrix4d upFix(1.0);
    if (UsdGeomGetStageUpAxis(stage) == UsdGeomTokens->z)
        upFix.SetRotate(GfRotation(GfVec3d(1, 0, 0), -90.0));

    auto scene = std::make_unique<Scene>();
    UsdGeomXformCache xfCache;

    for (const UsdPrim &prim : stage->Traverse()) {
        if (!prim.IsA<UsdGeomMesh>())
            continue;

        UsdGeomMesh mesh(prim);
        VtArray<GfVec3f> pts;
        VtArray<int32_t> counts;
        VtArray<int32_t> indices;
        mesh.GetPointsAttr().Get(&pts);
        mesh.GetFaceVertexCountsAttr().Get(&counts);
        mesh.GetFaceVertexIndicesAttr().Get(&indices);
        if (pts.empty() || counts.empty())
            continue;

        const GfMatrix4d world =
            xfCache.GetLocalToWorldTransform(prim) * upFix;

        ImportedMesh out;
        out.name = prim.GetName().GetString();
        out.points.reserve(pts.size() * 3);

        UsdGeomPrimvar st = UsdGeomPrimvarsAPI(mesh).GetPrimvar(TfToken("st"));
        VtArray<GfVec2f> uvValues;
        const bool hasUvs = st && st.ComputeFlattened(&uvValues) &&
                            uvValues.size() == indices.size();
        auto appendUv = [&](size_t index) {
            if (!hasUvs) return;
            out.uvs.push_back(uvValues[index][0]);
            out.uvs.push_back(uvValues[index][1]);
        };

        UsdShadeMaterial material = UsdShadeMaterialBindingAPI(mesh).ComputeBoundMaterial();
        if (material) {
            UsdShadeShader surface = material.ComputeSurfaceSource();
            if (surface) {
                GfVec3f color(1.0f);
                surface.GetInput(TfToken("diffuseColor")).Get(&color);
                out.material[0] = color[0]; out.material[1] = color[1]; out.material[2] = color[2];
                surface.GetInput(TfToken("opacity")).Get(&out.material[3]);
                surface.GetInput(TfToken("metallic")).Get(&out.material[4]);
                surface.GetInput(TfToken("roughness")).Get(&out.material[5]);
            }
            for (const UsdPrim &child : UsdPrimRange(material.GetPrim())) {
                UsdShadeShader shader(child);
                TfToken id;
                if (!shader || !shader.GetIdAttr().Get(&id) || id != TfToken("UsdUVTexture"))
                    continue;
                SdfAssetPath assetPath;
                if (!shader.GetInput(TfToken("file")).Get(&assetPath)) continue;
                const std::string anchored = SdfComputeAssetPathRelativeToLayer(
                    stage->GetRootLayer(), assetPath.GetAssetPath());
                auto asset = ArGetResolver().OpenAsset(ArResolvedPath(anchored));
                if (asset) {
                    out.texture.resize(asset->GetSize());
                    if (asset->Read(out.texture.data(), out.texture.size(), 0) != out.texture.size())
                        out.texture.clear();
                }
                break;
            }
        }
        for (const GfVec3f &p : pts) {
            const GfVec3d w = world.Transform(GfVec3d(p));
            out.points.push_back(static_cast<float>(w[0]));
            out.points.push_back(static_cast<float>(w[1]));
            out.points.push_back(static_cast<float>(w[2]));
        }

        // Keep tris/quads; fan-triangulate anything larger (Mesh2 has no
        // n-gons). Skip degenerate counts defensively.
        size_t cursor = 0;
        bool malformed = false;
        for (int32_t c : counts) {
            if (c < 3 || cursor + c > indices.size()) {
                malformed = true;
                break;
            }
            if (c <= 4) {
                out.counts.push_back(c);
                for (int32_t k = 0; k < c; ++k)
                    out.indices.push_back(indices[cursor + k]);
                for (int32_t k = 0; k < c; ++k)
                    appendUv(cursor + k);
            } else {
                for (int32_t k = 1; k + 1 < c; ++k) {
                    out.counts.push_back(3);
                    out.indices.push_back(indices[cursor]);
                    out.indices.push_back(indices[cursor + k]);
                    out.indices.push_back(indices[cursor + k + 1]);
                    appendUv(cursor);
                    appendUv(cursor + k);
                    appendUv(cursor + k + 1);
                }
            }
            cursor += c;
        }
        if (malformed || out.counts.empty())
            continue;

        scene->meshes.push_back(std::move(out));
    }

    std::remove(path.c_str());
    if (scene->meshes.empty()) {
        if (g_lastError.empty())
            g_lastError = "no polygon meshes found in file";
        return nullptr;
    }
    return scene.release();
}

uint32_t usdio_scene_mesh_count(void *h)
{
    return static_cast<Scene *>(h)->meshes.size();
}

const char *usdio_scene_mesh_name(void *h, uint32_t i)
{
    return static_cast<Scene *>(h)->meshes[i].name.c_str();
}

uint32_t usdio_mesh_num_points(void *h, uint32_t i)
{
    return static_cast<Scene *>(h)->meshes[i].points.size() / 3;
}

const float *usdio_mesh_points(void *h, uint32_t i)
{
    return static_cast<Scene *>(h)->meshes[i].points.data();
}

uint32_t usdio_mesh_num_faces(void *h, uint32_t i)
{
    return static_cast<Scene *>(h)->meshes[i].counts.size();
}

const int32_t *usdio_mesh_counts(void *h, uint32_t i)
{
    return static_cast<Scene *>(h)->meshes[i].counts.data();
}

uint32_t usdio_mesh_num_indices(void *h, uint32_t i)
{
    return static_cast<Scene *>(h)->meshes[i].indices.size();
}

uint32_t usdio_mesh_num_uvs(void *h, uint32_t i)
{
    return static_cast<Scene *>(h)->meshes[i].uvs.size() / 2;
}

const float *usdio_mesh_uvs(void *h, uint32_t i)
{
    return static_cast<Scene *>(h)->meshes[i].uvs.data();
}

uint32_t usdio_mesh_texture_size(void *h, uint32_t i)
{
    return static_cast<Scene *>(h)->meshes[i].texture.size();
}

const uint8_t *usdio_mesh_texture(void *h, uint32_t i)
{
    return static_cast<Scene *>(h)->meshes[i].texture.data();
}

const float *usdio_mesh_material(void *h, uint32_t i)
{
    return static_cast<Scene *>(h)->meshes[i].material;
}

const int32_t *usdio_mesh_indices(void *h, uint32_t i)
{
    return static_cast<Scene *>(h)->meshes[i].indices.data();
}

void usdio_scene_free(void *h) { delete static_cast<Scene *>(h); }

} // extern "C"
