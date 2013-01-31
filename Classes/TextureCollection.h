//
//  TextureCollection.h
//  MeshMaker
//
//  Created by Filip Kunc on 1/25/13.
//
//

#pragma once

#include "Texture.h"
#include "MemoryStream.h"
#include <vector>
using namespace std;

class ItemCollection;

class TextureCollection
{
private:
    vector<Texture *> _textures;
public:
    TextureCollection();
    TextureCollection(MemoryReadStream *stream);
    ~TextureCollection();
    
    void encode(MemoryWriteStream *stream);
    
    void addTexture(Texture *texture) { _textures.push_back(texture); }
    void removeTextureAtIndex(uint index, ItemCollection &items);
    Texture *textureAtIndex(uint index) { return _textures.at(index); }
    uint indexOfTexture(Texture *texture);
    uint count() { return _textures.size(); }
};
