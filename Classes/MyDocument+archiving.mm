//
//  MyDocument+archiving.mm
//  MeshMaker
//
//  Created by Filip Kunc on 11/16/12.
//
//

#import "MyDocument.h"
#import <sstream>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"
#import "rapidxml.hpp"
#pragma clang diagnostic pop

using namespace std;
using namespace rapidxml;

template <typename T>
vector<T> *ReadValues(string s)
{
    vector<T> *values = new vector<T>();
    
    istringstream ss(s);
    
    T value;
    
    while (ss >> value)
        values->push_back(value);
    
    return values;
}

@implementation MyDocument (Archiving)

- (BOOL)readFromFileWrapper:(NSFileWrapper *)dirWrapper ofType:(NSString *)typeName error:(NSError *__autoreleasing *)outError
{
    if ([typeName isEqualToString:@"model3D"])
        return [self readFromModel3D:[dirWrapper regularFileContents]];
    
    if ([typeName isEqualToString:@"Wavefront Object"])
        return [self readFromWavefrontObject:[dirWrapper regularFileContents]];
    
    if ([typeName isEqualToString:@"Collada"])
        return [self readFromCollada:[dirWrapper regularFileContents]];
    
    NSFileWrapper *modelWrapper = [[dirWrapper fileWrappers] objectForKey:@"Geometry.model3D"];
    NSData *modelData = [modelWrapper regularFileContents];
    [self readFromModel3D:modelData];
    
//    [[dirWrapper fileWrappers] enumerateKeysAndObjectsUsingBlock:^(id key, id obj, BOOL *stop)
//     {
//         NSString *textureName = (NSString *)key;
//         if ([textureName hasSuffix:@".png"])
//         {
//             textureName = [textureName stringByDeletingPathExtension];
//             
//             NSFileWrapper *textureWrapper = (NSFileWrapper *)obj;
//             NSData *textureData = [textureWrapper regularFileContents];
//             
//             NSImage *image = [[NSImage alloc] initWithData:textureData];
//             uint index = [textureName substringFromIndex:@"Texture".length].integerValue;
//             Item *item = items->itemAtIndex(index);
//             FPTexture *texture = item->mesh->texture();
//             [texture setCanvas:image];
//         }
//     }];
    
    return YES;
}

- (NSFileWrapper *)fileWrapperOfType:(NSString *)typeName error:(NSError *__autoreleasing *)outError
{
    if ([typeName isEqualToString:@"model3D"])
        return [[NSFileWrapper alloc] initRegularFileWithContents:[self dataOfModel3D]];
    
    if ([typeName isEqualToString:@"Wavefront Object"])
        return [[NSFileWrapper alloc] initRegularFileWithContents:[self dataOfWavefrontObject]];
    
    if ([typeName isEqualToString:@"Collada"])
        return [[NSFileWrapper alloc] initRegularFileWithContents:[self dataOfCollada]];
    
    NSFileWrapper *dirWrapper = [[NSFileWrapper alloc] initDirectoryWithFileWrappers:nil];
    
    [dirWrapper addRegularFileWithContents:[self dataOfModel3D]
                         preferredFilename:@"Geometry.model3D"];
    
    
//    for (int i = 0; i < (int)items->count(); i++)
//    {
//        NSImage *image = items->itemAtIndex(i)->mesh->texture().canvas;
//        NSBitmapImageRep *bitmap = [NSBitmapImageRep imageRepWithData:[image TIFFRepresentation]];
//        
//        NSData *imageData = [bitmap representationUsingType:NSPNGFileType properties:nil];
//        
//        
//        [dirWrapper addRegularFileWithContents:imageData
//                             preferredFilename:[NSString stringWithFormat:@"Texture%.2i.png", i]];
//    }
    
    return dirWrapper;
}

- (BOOL)readFromModel3D:(NSData *)data
{
    MemoryReadStream *stream = new MemoryReadStream(data);
    
    unsigned int version = 0;
    
    stream->readBytes(&version, sizeof(unsigned int));
    
    if (version < ModelVersionFirst || version > ModelVersionLatest)
        return NO;
    
    stream->setVersion(version);
    ItemCollection *newItems = new ItemCollection(stream);
    delete stream;
    items = newItems;
    
    itemsController->setModel(items);
    itemsController->updateSelection();
    [self setManipulated:itemsController];
    
    return YES;
}

- (NSData *)dataOfModel3D
{
    NSMutableData *data = [[NSMutableData alloc] init];
    MemoryWriteStream *stream = new MemoryWriteStream(data);
    
    unsigned int version = ModelVersionLatest;
    stream->setVersion(version);
    stream->writeBytes(&version, sizeof(unsigned int));
    items->encode(stream);

    delete stream;
    
    return data;
}

- (BOOL)readFromWavefrontObject:(NSData *)data
{
    NSString *fileContents = [NSString stringWithUTF8String:(const char *)[data bytes]];
    string str = [fileContents UTF8String];
    stringstream ssfile;
    ssfile << str;
    
    vector<Vector3D> vertices;
    vector<Vector3D> texCoords;
    vector<TriQuad> triangles;
    vector<uint> groups;
    
    bool hasTexCoords = false;
    bool hasNormals = false;
    
    while (!ssfile.eof())
    {
        string line;
        getline(ssfile, line);
        stringstream ssline;
        ssline << line;
        
        string prefix;
        ssline >> prefix;
        
        if (prefix == "#")
        {
            // # This is a comment
            continue;
        }
        else if (prefix == "g")
        {
            // g group_name
            groups.push_back(triangles.size());
        }
        else if (prefix == "v")
        {
            // v -5.79346 -1.38018 42.63113
            Vector3D v;
            ssline >> v.x >> v.y >> v.z;
            
            swap(v.y, v.z);
            v.z = -v.z;
            
            vertices.push_back(v);
        }
        else if (prefix == "vt")
        {
            // vt 0.12528 -0.64560
            Vector3D vt;
            ssline >> vt.x >> vt.y >> vt.z;
            
            vt.z = 0.0f;
            
            texCoords.push_back(vt);
            hasTexCoords = true;
        }
        else if (prefix == "vn")
        {
            // vn -0.78298 -0.13881 -0.60637
            hasNormals = true;
        }
        else if (prefix == "f")
        {
            // f  v1 v2 v3 v4 ...
            // f  v1/vt1 v2/vt2 v3/vt3 ...
            // f  v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ...
            // f  v1//vn1 v2//vn2 v3//vn3 ...
            
            // f  187/1/1 204/2/2 185/3/3
            
            TriQuad triQuad;
            for (uint i = 0; i < 4; i++)
            {
                uint vi = 0, ti = 0, ni = 0;
                char c;
                
                if (!hasTexCoords && !hasNormals)
                    ssline >> vi;
                else if (hasTexCoords && !hasNormals)
                    ssline >> vi >> c >> ti;
                else if (!hasTexCoords && hasNormals)
                    ssline >> vi >> c >> c >> ni;
                else if (hasTexCoords && hasNormals)
                    ssline >> vi >> c >> ti >> c >> ni;
                
                triQuad.vertexIndices[i] = vi == 0 ? 0 : vi - 1;
                triQuad.texCoordIndices[i] = ti == 0 ? 0 : ti - 1;
            }
            triQuad.isQuad = ssline.good();
            triangles.push_back(triQuad);
        }
    }
    
    Mesh2 *mesh = new Mesh2();
    if (!hasTexCoords)
        mesh->fromIndexRepresentation(vertices, vertices, triangles);
    else
        mesh->fromIndexRepresentation(vertices, texCoords, triangles);
    
    mesh->flipAllTriangles();
    
    mesh->setSelectionMode(MeshSelectionModeTriangles);
    
    ItemCollection *newItems = new ItemCollection();
    
    for (uint i = 0; i < groups.size(); i++)
    {
        for (uint j = 0; j < mesh->triangleCount(); j++)
            mesh->setSelectedAtIndex(false, j);
        
        for (uint j = groups.at(i), end = i + 1 < groups.size() ? groups.at(i + 1) : mesh->triangleCount(); j < end; j++)
            mesh->setSelectedAtIndex(true, j);
        
        Item *item = new Item(new Mesh2());
        mesh->fillMeshFromSelectedTriangles(*item->mesh);
        item->setPositionToGeometricCenter();
        newItems->addItem(item);
    }
    
    if (groups.empty())
    {
        Item *item = new Item(mesh);
        item->setPositionToGeometricCenter();
        newItems->addItem(item);
    }
    else
    {
        delete mesh;
    }
    
    delete items;
    items = newItems;
    
    itemsController->setModel(items);
    itemsController->updateSelection();
    [self setManipulated:itemsController];
    
    return YES;
}

- (NSData *)dataOfWavefrontObject
{
    if (items->count() == 0)
        return [@"# Nothing to export" dataUsingEncoding:NSUTF8StringEncoding];
    
    stringstream ssfile;
    
    NSString *version = [[[NSBundle mainBundle] infoDictionary] valueForKey:@"CFBundleVersion"];
    ssfile << "# Exported from MeshMaker " << [version UTF8String] << endl;
    
    // face indices in Wavefront Object starts from 1
    uint vertexIndexOffset = 1;
    uint texCoordIndexOffset = 1;
    
    for (uint itemIndex = 0; itemIndex < items->count(); itemIndex++)
    {
        Item *item = items->itemAtIndex(itemIndex);
        
        vector<Vector3D> vertices;
        vector<Vector3D> texCoords;
        vector<TriQuad> triangles;
        
        Item *duplicate = item->duplicate();
        Mesh2 *mesh = duplicate->mesh;
        mesh->transformAll(duplicate->transform());
        mesh->flipAllTriangles();
        mesh->toIndexRepresentation(vertices, texCoords, triangles);
        
        ssfile << "g Item_" << itemIndex << endl;
        ssfile << "# Number of vertices = " << vertices.size() << endl;
        for (uint i = 0; i < vertices.size(); i++)
        {
            // v -5.79346 -1.38018 42.63113
            Vector3D v = vertices[i];
            v.z = -v.z;
            swap(v.y, v.z);
            ssfile << "v " << v.x << " " << v.y << " " << v.z << endl;
        }
        
        ssfile << "# Number of texture coordinates = " << texCoords.size() << endl;
        for (uint i = 0; i < texCoords.size(); i++)
        {
            // vt 0.12528 -0.64560
            ssfile << "vt " << texCoords[i].x << " " << texCoords[i].y << " " << endl;
        }
        
        ssfile << "# Number of triangles and quads = " << triangles.size() << endl;
        for (uint i = 0; i < triangles.size(); i++)
        {
            // f  v1/vt1 v2/vt2 v3/vt3 ...
            ssfile << "f ";
            const TriQuad &triQuad = triangles[i];
            uint count =  triQuad.isQuad ? 4 : 3;
            for (uint i = 0; i < count; i++)
            {
                ssfile << triQuad.vertexIndices[i] + vertexIndexOffset << "/";
                ssfile << triQuad.texCoordIndices[i] + texCoordIndexOffset << " ";
            }
            ssfile << endl;
        }
        
        vertexIndexOffset += vertices.size();
        texCoordIndexOffset += texCoords.size();
    }
    
    string str = ssfile.str();
    return [[NSString stringWithUTF8String:str.c_str()] dataUsingEncoding:NSUTF8StringEncoding];
}

- (void)readMesh:(Mesh2 *)itemMesh fromXml:(xml_node< > *)meshXml
{
    string positionsString = meshXml->first_node("source")->first_node("float_array")->value();
    vector<float> *points = ReadValues<float>(positionsString);
    
    string uvCoordsString = meshXml->first_node("source")->next_sibling()->next_sibling()->first_node("float_array")->value();
    vector<float> *uvCoords = ReadValues<float>(uvCoordsString);
    
    xml_node< > *triNode = meshXml->first_node("source")->next_sibling("triangles")->first_node();
    
    uint inputTypesCount = 0;
    string trianglesString;
    
    while (true)
    {
        if (strcmp(triNode->name(), "p") == 0)
        {
            trianglesString = triNode->value();
            break;
        }
        else
        {
            triNode = triNode->next_sibling();
            inputTypesCount++;
        }
    }
    
    vector<uint> *indices = ReadValues<uint>(trianglesString);
    
    vector<Vector3D> vertices;
    vector<Vector3D> texCoords;
    vector<TriQuad> triangles;
    
    uint pointsSize = points->size();
    
    for (uint i = 0; i < pointsSize; i += 3)
    {
        Vector3D point;
        for (uint j = 0; j < 3; j++)
            point[j] = points->at(i + j);
        
        vertices.push_back(point);
    }
    
    for (uint i = 0; i < uvCoords->size(); i += 2)
    {
        Vector3D uvCoord;
        for (uint j = 0; j < 2; j++)
            uvCoord[j] = (*uvCoords)[i + j];
        
        texCoords.push_back(uvCoord);
    }
    
    vector<uint> &trianglesRef = *indices;
    
    for (uint i = 0; i < trianglesRef.size(); i += inputTypesCount * 3)
    {
        uint vertexIndices[3];
        uint texCoordIndices[3];
        
        for (uint j = 0; j < 3; j++)
        {
            vertexIndices[j] = trianglesRef.at(i + j * inputTypesCount);
            texCoordIndices[j] = trianglesRef.at(i + j * inputTypesCount + inputTypesCount - 1);
        }
        
        AddTriangle(triangles, vertexIndices, texCoordIndices);
    }
    
    itemMesh->fromIndexRepresentation(vertices, texCoords, triangles);
    itemMesh->flipAllTriangles();
    
    delete points;
    delete uvCoords;
    delete indices;
}

- (BOOL)readFromCollada:(NSData *)data
{
    NSString* xmlData = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
    
    uint length = [xmlData lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
    char *textBuffer = new char [length + 1];
    memset(textBuffer, 0, length + 1);
    memcpy(textBuffer, [xmlData UTF8String], length);
    
    xml_document< > document;
    document.parse<0>(textBuffer);
    
    xml_node< > *geometries = document.first_node()->first_node("library_geometries");
    xml_node< > *visualScenes = document.first_node()->first_node("library_visual_scenes")->first_node("visual_scene");
    
    ItemCollection *newItems = new ItemCollection();
    
    for (xml_node< > *node = visualScenes->first_node("node"); node; node = node->next_sibling())
    {
        xml_node< > *instanceGeometry = node->first_node("instance_geometry");
        if (instanceGeometry != NULL)
        {
            Item *item = new Item(new Mesh2());
            
            xml_attribute< > *url = instanceGeometry->first_attribute("url");
            char *urlValue = url->value();
            urlValue++; // Skipping '#'
            
            for (xml_node< > *geometry = geometries->first_node("geometry"); geometry; geometry = geometry->next_sibling())
            {
                if (strcmp(urlValue, geometry->first_attribute("id")->value()) == 0)
                {
                    [self readMesh:item->mesh fromXml:geometry->first_node("mesh")];
                    break;
                }
            }
            
            xml_node< > *translate = node->first_node("translate");
            if (translate != NULL)
            {
                float x, y, z;
                sscanf(translate->value(), "%f %f %f", &x, &y, &z);
                item->position = Vector3D(x, y, z);
            }
            
            newItems->addItem(item);
        }
    }
    
    delete items;
    items = newItems;
    itemsController->setModel(items);
    itemsController->updateSelection();
    [self setManipulated:itemsController];
    
    delete [] textBuffer;
    
    return YES;
}

- (NSData *)dataOfCollada
{
    NSMutableString *colladaXml = [[NSMutableString alloc] init];
    
    [colladaXml appendString:@"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"];
    [colladaXml appendString:@"<COLLADA xmlns=\"http://www.collada.org/2005/11/COLLADASchema\" version=\"1.4.1\">\n"];
    {
        [colladaXml appendString:@"<asset>\n"];
        {
            [colladaXml appendString:@"<contributor>\n"];
            {
                NSString *version =[[[NSBundle mainBundle] infoDictionary] valueForKey:@"CFBundleVersion"];
                [colladaXml appendFormat:@"<authoring_tool>MeshMaker %@</authoring_tool>\n", version];
            }
            [colladaXml appendString:@"</contributor>\n"];
            [colladaXml appendString:@"<created>2011-01-23T15:41:29Z</created>\n"];     // TODO: fill real date-time
            [colladaXml appendString:@"<modified>2011-01-23T15:41:29Z</modified>\n"];   // TODO: fill real date-time
            [colladaXml appendString:@"<up_axis>Y_UP</up_axis>\n"];
        }
        [colladaXml appendString:@"</asset>\n"];
        
        [colladaXml appendString:@"<library_cameras>\n"];
        {
            [colladaXml appendString:@"<camera id=\"Camera-Camera\" name=\"Camera\">\n"];
            {
                [colladaXml appendString:@"<optics>\n"];
                {
                    [colladaXml appendString:@"<technique_common>\n"];
                    {
                        [colladaXml appendString:@"<perspective>\n"];
                        {
                            [colladaXml appendString:@"<xfov sid=\"HFOV\">39.5978</xfov>\n"];       // TODO: fill real HFOV
                            [colladaXml appendString:@"<yfov sid=\"YFOV\">26.9915</yfov>\n"];       // TODO: fill real YFOV
                            [colladaXml appendString:@"<znear sid=\"near_clip\">0.01</znear>\n"];   // TODO: fill real near_clip
                            [colladaXml appendString:@"<zfar sid=\"far_clip\">10000</zfar>\n"];     // TODO: fill real far_clip
                        }
                        [colladaXml appendString:@"</perspective>\n"];
                    }
                    [colladaXml appendString:@"</technique_common>\n"];
                }
                [colladaXml appendString:@"</optics>\n"];
            }
            [colladaXml appendString:@"</camera>\n"];
        }
        [colladaXml appendString:@"</library_cameras>\n"];
        
        [colladaXml appendString:@"<library_materials>\n"];
        {
            [colladaXml appendString:@"<material id=\"Material-Default\" name=\"Default\">\n"];
            {
                [colladaXml appendString:@"<instance_effect url=\"#Effect-Default\" />\n"];
            }
            [colladaXml appendString:@"</material>\n"];
        }
        [colladaXml appendString:@"</library_materials>\n"];
        
        [colladaXml appendString:@"<library_effects>\n"];
        {
            [colladaXml appendString:@"<effect id=\"Effect-Default\" name=\"Default\">\n"];
            {
                [colladaXml appendString:@"<profile_COMMON>\n"];
                {
                    [colladaXml appendString:@"<technique sid=\"common\">\n"];
                    {
                        [colladaXml appendString:@"<phong>\n"];
                        {
                            [colladaXml appendString:@"<diffuse>\n"];
                            {
                                [colladaXml appendString:@"<color sid=\"diffuse_effect_rgb\">0.8 0.8 0.8 1</color>\n"];
                            }
                            [colladaXml appendString:@"</diffuse>\n"];
                            
                            [colladaXml appendString:@"<specular>\n"];
                            {
                                [colladaXml appendString:@"<color sid=\"specular_effect_rgb\">0.2 0.2 0.2 1</color>\n"];
                            }
                            [colladaXml appendString:@"</specular>\n"];
                        }
                        [colladaXml appendString:@"</phong>\n"];
                    }
                    [colladaXml appendString:@"</technique>\n"];
                }
                [colladaXml appendString:@"</profile_COMMON>\n"];
            }
            [colladaXml appendString:@"</effect>\n"];
        }
        [colladaXml appendString:@"</library_effects>\n"];
        
        [colladaXml appendString:@"<library_geometries>\n"];
        {
            for (uint itemID = 0; itemID < items->count(); itemID++)
            {
                Item *item = items->itemAtIndex(itemID);
                
                vector<Vector3D> vertices;
                vector<Vector3D> texCoords;
                vector<Vector3D> normals;
                vector<TriQuad> triangles;
                
                Item *duplicate = item->duplicate();
                Mesh2 *mesh = duplicate->mesh;
                mesh->transformAll(duplicate->transform());
                
                mesh->triangulate();
                mesh->flipAllTriangles();
                mesh->toIndexRepresentation(vertices, texCoords, triangles);
                mesh->fillTriangleCache();
                
                const FPList<VertexNode, Vertex2> &verticesRef = mesh->vertices();
                for (VertexNode *node = verticesRef.begin(), *end = verticesRef.end(); node != end; node = node->next())
                    normals.push_back(node->algorithmData.normal);
                
                [colladaXml appendFormat:@"<geometry id=\"Geometry-Mesh_%i\" name=\"Mesh_%i\">\n", itemID, itemID];
                {
                    [colladaXml appendString:@"<mesh>\n"];
                    {
                        // positions
                        [colladaXml appendFormat:@"<source id=\"Geometry-Mesh_%i-positions\" name=\"positions\">\n", itemID];
                        {
                            [colladaXml appendFormat:@"<float_array id=\"Geometry-Mesh_%i-positions-array\" count=\"%li\">\n",
                             itemID, vertices.size() * 3];
                            {
                                for (uint i = 0; i < vertices.size(); i++)
                                    [colladaXml appendFormat:@"%f %f %f\n", vertices[i].x, vertices[i].y, vertices[i].z];
                            }
                            [colladaXml appendString:@"</float_array>\n"];
                            
                            [colladaXml appendString:@"<technique_common>\n"];
                            {
                                [colladaXml appendFormat:@"<accessor count=\"%li\" source=\"#Geometry-Mesh_%i-positions-array\" stride=\"3\">\n",
                                 vertices.size(), itemID];
                                {
                                    [colladaXml appendString:@"<param name=\"X\" type=\"float\" />\n"];
                                    [colladaXml appendString:@"<param name=\"Y\" type=\"float\" />\n"];
                                    [colladaXml appendString:@"<param name=\"Z\" type=\"float\" />\n"];
                                }
                                [colladaXml appendString:@"</accessor>\n"];
                            }
                            [colladaXml appendString:@"</technique_common>\n"];
                        }
                        [colladaXml appendString:@"</source>\n"];
                        
                        // normals
                        [colladaXml appendFormat:@"<source id=\"Geometry-Mesh_%i-normals\" name=\"normals\">\n", itemID];
                        {
                            [colladaXml appendFormat:@"<float_array id=\"Geometry-Mesh_%i-normals-array\" count=\"%li\">\n",
                             itemID, normals.size() * 3];
                            {
                                for (uint i = 0; i < normals.size(); i++)
                                    [colladaXml appendFormat:@"%f %f %f\n", normals[i].x, normals[i].y, normals[i].z];
                            }
                            [colladaXml appendString:@"</float_array>\n"];
                            
                            [colladaXml appendString:@"<technique_common>\n"];
                            {
                                [colladaXml appendFormat:@"<accessor count=\"%li\" source=\"#Geometry-Mesh_%i-normals-array\" stride=\"3\">\n",
                                 normals.size(), itemID];
                                {
                                    [colladaXml appendString:@"<param name=\"X\" type=\"float\" />\n"];
                                    [colladaXml appendString:@"<param name=\"Y\" type=\"float\" />\n"];
                                    [colladaXml appendString:@"<param name=\"Z\" type=\"float\" />\n"];
                                }
                                [colladaXml appendString:@"</accessor>\n"];
                            }
                            [colladaXml appendString:@"</technique_common>\n"];
                        }
                        [colladaXml appendString:@"</source>\n"];
                        
                        // texture
                        [colladaXml appendFormat:@"<source id=\"Geometry-Mesh_%i-Texture\" name=\"Texture\">\n", itemID];
                        {
                            [colladaXml appendFormat:@"<float_array id=\"Geometry-Mesh_%i-Texture-array\" count=\"%li\">\n",
                             itemID, texCoords.size() * 2];
                            {
                                for (uint i = 0; i < texCoords.size(); i++)
                                    [colladaXml appendFormat:@"%f %f\n", texCoords[i].x, texCoords[i].y];
                            }
                            [colladaXml appendString:@"</float_array>\n"];
                            
                            [colladaXml appendString:@"<technique_common>\n"];
                            {
                                [colladaXml appendFormat:@"<accessor count=\"%li\" source=\"#Geometry-Mesh_%i-Texture-array\" stride=\"2\">\n",
                                 texCoords.size(), itemID];
                                {
                                    [colladaXml appendString:@"<param name=\"S\" type=\"float\" />\n"];
                                    [colladaXml appendString:@"<param name=\"T\" type=\"float\" />\n"];
                                }
                                [colladaXml appendString:@"</accessor>\n"];
                            }
                            [colladaXml appendString:@"</technique_common>\n"];
                        }
                        [colladaXml appendString:@"</source>\n"];
                        
                        [colladaXml appendFormat:@"<vertices id=\"Geometry-Mesh_%i-vertices\">\n", itemID];
                        {
                            [colladaXml appendFormat:@"<input semantic=\"POSITION\" source=\"#Geometry-Mesh_%i-positions\" />\n", itemID];
                        }
                        [colladaXml appendString:@"</vertices>\n"];
                        
                        [colladaXml appendFormat:@"<triangles count=\"%li\" material=\"Material-Default\">\n", triangles.size()];
                        {
                            [colladaXml appendFormat:@"<input semantic=\"VERTEX\" source=\"#Geometry-Mesh_%i-vertices\" offset=\"0\" />\n", itemID];
                            [colladaXml appendFormat:@"<input semantic=\"NORMAL\" source=\"#Geometry-Mesh_%i-normals\" offset=\"1\" />\n", itemID];
                            [colladaXml appendFormat:@"<input semantic=\"TEXCOORD\" source=\"#Geometry-Mesh_%i-Texture\" offset=\"2\" set=\"0\" />\n", itemID];
                            
                            [colladaXml appendString:@"<p>"];
                            {
                                for (uint i = 0; i < triangles.size(); i++)
                                {
                                    const TriQuad &t = triangles[i];
                                    // TODO: implement tri/quads.
                                    
                                    for (uint j = 0; j < 3; j++)
                                        [colladaXml appendFormat:@"%i %i %i ", t.vertexIndices[j], t.vertexIndices[j], t.texCoordIndices[j]];
                                }
                            }
                            [colladaXml appendString:@"</p>\n"];
                        }
                        [colladaXml appendString:@"</triangles>\n"];
                    }
                    [colladaXml appendString:@"</mesh>\n"];
                }
                [colladaXml appendString:@"</geometry>\n"];                
            }
        }
        [colladaXml appendString:@"</library_geometries>\n"];
        
        [colladaXml appendString:@"<library_lights>\n"];
        {
            [colladaXml appendString:@"<light id=\"Light-Render\" name=\"Render\">\n"];
            {
                [colladaXml appendString:@"<technique_common>\n"];
                {
                    [colladaXml appendString:@"<ambient>\n"];
                    {
                        [colladaXml appendString:@"<color sid=\"ambient_light_rgb\">0.05 0.05 0.05</color>\n"];
                    }
                    [colladaXml appendString:@"</ambient>\n"];
                }
                [colladaXml appendString:@"</technique_common>\n"];
            }
            [colladaXml appendString:@"</light>\n"];
            [colladaXml appendString:@"<light id=\"Light-Directional_Light\" name=\"Directional_Light\">\n"];
            {
                [colladaXml appendString:@"<technique_common>\n"];
                {
                    [colladaXml appendString:@"<directional>\n"];
                    {
                        [colladaXml appendString:@"<color sid=\"directional_light_rgb\">1 1 1</color>\n"];
                    }
                    [colladaXml appendString:@"</directional>\n"];
                }
                [colladaXml appendString:@"</technique_common>\n"];
            }
            [colladaXml appendString:@"</light>\n"];
        }
        [colladaXml appendString:@"</library_lights>\n"];
        
        [colladaXml appendString:@"<library_visual_scenes>\n"];
        {
            [colladaXml appendString:@"<visual_scene id=\"DefaultScene\">\n"];
            {
                [colladaXml appendString:@"<node id=\"RenderNode\" name=\"Render\" type=\"NODE\">\n"];
                {
                    [colladaXml appendString:@"<instance_light url=\"#Light-Render\" />\n"];
                }
                [colladaXml appendString:@"</node>\n"];
                
                for (uint itemID = 0; itemID < items->count(); itemID++)
                {
                    [colladaXml appendFormat:@"<node id=\"Geometry-MeshNode_%i\" name=\"Mesh_%i\" type=\"NODE\">\n", itemID, itemID];
                    {
                        // TODO: fill rotation and scale too
                        
                        [colladaXml appendFormat:@"<translate sid=\"Position_%i\">%f %f %f</translate>\n",
                         itemID, 0.0f, 0.0f, 0.0f];
                        
                        [colladaXml appendFormat:@"<instance_geometry url=\"#Geometry-Mesh_%i\">\n", itemID];
                        {
                            [colladaXml appendString:@"<bind_material>\n"];
                            {
                                [colladaXml appendString:@"<technique_common>\n"];
                                {
                                    [colladaXml appendString:@"<instance_material symbol=\"Material-Default\" target=\"#Material-Default\" />\n"];
                                }
                                [colladaXml appendString:@"</technique_common>\n"];
                            }
                            [colladaXml appendString:@"</bind_material>\n"];
                        }
                        [colladaXml appendString:@"</instance_geometry>\n"];
                    }
                    [colladaXml appendString:@"</node>\n"];
                }
                
                [colladaXml appendString:@"<node id=\"Camera-CameraNode\" name=\"Camera\" type=\"NODE\">\n"];
                {
                    [colladaXml appendString:@"<translate sid=\"Position\">0 0.75 10</translate>\n"];
                    [colladaXml appendString:@"<rotate sid=\"RotationY\">0 1 0 0</rotate>\n"];
                    [colladaXml appendString:@"<rotate sid=\"RotationX\">1 0 0 -5</rotate>\n"];
                    [colladaXml appendString:@"<rotate sid=\"RotationZ\">0 0 1 0</rotate>\n"];
                    [colladaXml appendString:@"<instance_camera url=\"#Camera-Camera\" />\n"];
                }
                [colladaXml appendString:@"</node>\n"];
                
                [colladaXml appendString:@"<node id=\"Light-Directional_LightNode\" name=\"Directional_Light\" type=\"NODE\">\n"];
                {
                    [colladaXml appendString:@"<translate sid=\"Position__2_\">-2 2 2</translate>\n"];
                    [colladaXml appendString:@"<rotate sid=\"Rotation__2_Y\">0 1 0 -45</rotate>\n"];
                    [colladaXml appendString:@"<rotate sid=\"Rotation__2_X\">1 0 0 -30</rotate>\n"];
                    [colladaXml appendString:@"<rotate sid=\"Rotation__2_Z\">0 0 1 0</rotate>\n"];
                    [colladaXml appendString:@"<instance_light url=\"#Light-Directional_Light\" />\n"];
                }
                [colladaXml appendString:@"</node>\n"];
            }
            [colladaXml appendString:@"</visual_scene>\n"];
        }
        [colladaXml appendString:@"</library_visual_scenes>\n"];
        
        [colladaXml appendString:@"<scene>\n"];
        {
            [colladaXml appendString:@"<instance_visual_scene url=\"#DefaultScene\" />\n"];
        }
        [colladaXml appendString:@"</scene>\n"];
    }
    [colladaXml appendString:@"</COLLADA>\n"];
    
    return [colladaXml dataUsingEncoding:NSUTF8StringEncoding];
}

@end
