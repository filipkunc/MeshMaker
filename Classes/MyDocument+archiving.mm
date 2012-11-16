//
//  MyDocument+archiving.mm
//  MeshMaker
//
//  Created by Filip Kunc on 11/16/12.
//
//

#import "MyDocument.h"
#import <sstream>

@implementation MyDocument (Archiving)

- (BOOL)readFromFileWrapper:(NSFileWrapper *)dirWrapper ofType:(NSString *)typeName error:(NSError *__autoreleasing *)outError
{
    if ([typeName isEqualToString:@"model3D"])
        return [self readFromModel3D:[dirWrapper regularFileContents]];
    
    if ([typeName isEqualToString:@"Wavefront Object"])
        return [self readFromWavefrontObject:[dirWrapper regularFileContents]];
    
    NSFileWrapper *modelWrapper = [[dirWrapper fileWrappers] objectForKey:@"Geometry.model3D"];
    NSData *modelData = [modelWrapper regularFileContents];
    [self readFromModel3D:modelData];
    
    [[dirWrapper fileWrappers] enumerateKeysAndObjectsUsingBlock:^(id key, id obj, BOOL *stop)
     {
         NSString *textureName = (NSString *)key;
         if ([textureName hasSuffix:@".png"])
         {
             textureName = [textureName stringByDeletingPathExtension];
             
             NSFileWrapper *textureWrapper = (NSFileWrapper *)obj;
             NSData *textureData = [textureWrapper regularFileContents];
             
             NSImage *image = [[NSImage alloc] initWithData:textureData];
             uint index = [textureName substringFromIndex:@"Texture".length].integerValue;
             Item *item = [items itemAtIndex:index];
             FPTexture *texture = item.mesh->texture();
             [texture setCanvas:image];
         }
     }];
    
    return YES;
}

- (NSFileWrapper *)fileWrapperOfType:(NSString *)typeName error:(NSError *__autoreleasing *)outError
{
    if ([typeName isEqualToString:@"model3D"])
        return [[NSFileWrapper alloc] initRegularFileWithContents:[self dataOfModel3D]];
    
    if ([typeName isEqualToString:@"Wavefront Object"])
        return [[NSFileWrapper alloc] initRegularFileWithContents:[self dataOfWavefrontObject]];
    
    NSFileWrapper *dirWrapper = [[NSFileWrapper alloc] initDirectoryWithFileWrappers:nil];
    
    [dirWrapper addRegularFileWithContents:[self dataOfModel3D]
                         preferredFilename:@"Geometry.model3D"];
    
    int i = 0;
    
    for (Item *item in items)
    {
        NSImage *image = item.mesh->texture().canvas;
        NSBitmapImageRep *bitmap = [NSBitmapImageRep imageRepWithData:[image TIFFRepresentation]];
        
        NSData *imageData = [bitmap representationUsingType:NSPNGFileType properties:nil];
        
        
        [dirWrapper addRegularFileWithContents:imageData
                             preferredFilename:[NSString stringWithFormat:@"Texture%.2i.png", i]];
        
        i++;
    }
    
    return dirWrapper;
}

- (BOOL)readFromModel3D:(NSData *)data
{
    MemoryReadStream *stream = [[MemoryReadStream alloc] initWithData:data];
    
    unsigned int version = 0;
    
    [stream readBytes:&version length:sizeof(unsigned int)];
    
    if (version < ModelVersionFirst || version > ModelVersionLatest)
        return NO;
    
    [stream setVersion:version];
    ItemCollection *newItems = [[ItemCollection alloc] initWithReadStream:stream];
    items = newItems;
    [itemsController setModel:items];
    [itemsController updateSelection];
    [self setManipulated:itemsController];
    
    return YES;
}

- (NSData *)dataOfModel3D
{
    NSMutableData *data = [[NSMutableData alloc] init];
    MemoryWriteStream *stream = [[MemoryWriteStream alloc] initWithData:data];
    
    unsigned int version = ModelVersionLatest;
    [stream setVersion:version];
    [stream writeBytes:&version length:sizeof(unsigned int)];
    [items encodeWithWriteStream:stream];
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
                uint vi, ti, ni;
                char c;
                
                if (!hasTexCoords && !hasNormals)
                    ssline >> vi;
                else if (hasTexCoords && !hasNormals)
                    ssline >> vi >> c >> ti;
                else if (!hasTexCoords && hasNormals)
                    ssline >> vi >> c >> c >> ni;
                else if (hasTexCoords && hasNormals)
                    ssline >> vi >> c >> ti >> c >> ni;
                
                triQuad.vertexIndices[i] = vi - 1;
                triQuad.texCoordIndices[i] = ti - 1;
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
    
    ItemCollection *newItems = [[ItemCollection alloc] init];
    
    for (uint i = 0; i < groups.size(); i++)
    {
        for (uint j = 0; j < mesh->triangleCount(); j++)
            mesh->setSelectedAtIndex(false, j);
        
        for (uint j = groups.at(i), end = i + 1 < groups.size() ? groups.at(i + 1) : mesh->triangleCount(); j < end; j++)
            mesh->setSelectedAtIndex(true, j);
        
        Item *item = [[Item alloc] initFromSelectedTrianglesInMesh:mesh];
        [item setPositionToGeometricCenter];
        [newItems addItem:item];
    }
    
    if (groups.empty())
    {
        Item *item = [[Item alloc] initWithMesh:mesh];
        [item setPositionToGeometricCenter];
        [newItems addItem:item];
    }
    else
    {
        delete mesh;
    }
    
    items = newItems;
    [itemsController setModel:items];
    [itemsController updateSelection];
    [self setManipulated:itemsController];
    
    return YES;
}

- (NSData *)dataOfWavefrontObject
{
    if (items.count == 0)
        return [@"# Nothing to export" dataUsingEncoding:NSUTF8StringEncoding];
    
    stringstream ssfile;
    ssfile << "# Exported from MeshMaker 1.2" << endl;
    
    // face indices in Wavefront Object starts from 1
    uint vertexIndexOffset = 1;
    uint texCoordIndexOffset = 1;
    uint itemIndex = 0;
    
    for (Item *item in items)
    {
        vector<Vector3D> vertices;
        vector<Vector3D> texCoords;
        vector<TriQuad> triangles;
        
        Item *duplicate = item.duplicate;
        Mesh2 *mesh = duplicate.mesh;
        mesh->transformAll(duplicate.transform);
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
        itemIndex++;
    }
    
    string str = ssfile.str();
    return [[NSString stringWithUTF8String:str.c_str()] dataUsingEncoding:NSUTF8StringEncoding];
}

@end
