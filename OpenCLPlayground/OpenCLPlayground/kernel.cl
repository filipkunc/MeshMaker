
typedef struct
{
    float3 a;
    float3 b;
    float3 c;
}  Triangle;

kernel void computeNormals(global Triangle *triangles, global float3 *normals)
{
    int i = get_global_id(0);
    float3 u = triangles[i].a - triangles[i].b;
    float3 v = triangles[i].b - triangles[i].c;
    normals[i] = normalize(cross(u, v));
}
