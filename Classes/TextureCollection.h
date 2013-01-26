//
//  TextureCollection.h
//  MeshMaker
//
//  Created by Filip Kunc on 1/25/13.
//
//

#pragma once

#include "Texture.h"
#include <vector>
using namespace std;

class TextureCollection
{
private:
    vector<Texture *> _textures;
public:
    TextureCollection();
    ~TextureCollection();
    
    void addTexture(Texture *texture) { _textures.push_back(texture); }
    Texture *textureAtIndex(uint index) { return _textures.at(index); }
    uint count() { return _textures.size(); }
};
