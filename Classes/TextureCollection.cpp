//
//  TextureCollection.cpp
//  MeshMaker
//
//  Created by Filip Kunc on 1/25/13.
//
//

#include "TextureCollection.h"
#include "ItemCollection.h"

TextureCollection::TextureCollection()
{
    
}

TextureCollection::~TextureCollection()
{
    for (unsigned int i = 0; i < _textures.size(); i++)
    {
        delete _textures[i];
    }
    
    _textures.clear();
}

void TextureCollection::removeTextureAtIndex(unsigned int index, ItemCollection &items)
{
    Texture *texture = _textures.at(index);
    texture->removeFromItems(items);
    _textures.erase(_textures.begin() + index);
    delete texture;
}

unsigned int TextureCollection::indexOfTexture(Texture *texture)
{
    for (unsigned int i = 0; i < _textures.size(); i++)
    {
        if (texture == _textures[i])
            return i;
    }
    return UINT_MAX;
}

TextureCollection::TextureCollection(MemoryReadStream *stream)
{
    ModelVersion version = (ModelVersion)stream->version();
    if (version >= ModelVersion::TextureNames)
    {
        unsigned int textureCount = stream->read<unsigned int>();
        for (unsigned int i = 0; i < textureCount; i++)
        {
            unsigned int charCount = stream->read<unsigned int>();
            char *utf8String = (char *)malloc(charCount + 1);
            memset(utf8String, 0, charCount + 1);
            stream->readBytes(utf8String, charCount);
            NSString *name = [NSString stringWithUTF8String:utf8String];
            free(utf8String);
            Texture *texture = new Texture();
            texture->setName(name);

            // image will be opened if it is in Pictures folder or inside .folder3D
            // there is currently no security scoped bookmarks to image file
            NSImage *image = [[NSImage alloc] initWithContentsOfFile:name];

            if (image)
                texture->setImage(image);
            
            addTexture(texture);
        }
    }
}

void TextureCollection::encode(MemoryWriteStream *stream)
{
    unsigned int textureCount = _textures.size();
    stream->write<unsigned int>(textureCount);
    for (unsigned int i = 0; i < textureCount; i++)
    {
        NSString *name = _textures[i]->name();
        
        unsigned int charCount = [name lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
        const char *utf8String = [name UTF8String];		

        stream->write<unsigned int>(charCount);
        stream->writeBytes(utf8String, charCount);
    }
}
