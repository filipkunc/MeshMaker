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
    void removeTextureAtIndex(unsigned int index, ItemCollection &items);
    Texture *textureAtIndex(unsigned int index) { return _textures.at(index); }
    unsigned int indexOfTexture(Texture *texture);
    unsigned int count() { return _textures.size(); }
};
