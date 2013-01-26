//
//  TextureCollection.cpp
//  MeshMaker
//
//  Created by Filip Kunc on 1/25/13.
//
//

#include "TextureCollection.h"

TextureCollection::TextureCollection()
{
    
}

TextureCollection::~TextureCollection()
{
    for (uint i = 0; i < _textures.size(); i++)
    {
        delete _textures[i];
    }
    
    _textures.clear();
}