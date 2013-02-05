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
    for (uint i = 0; i < _textures.size(); i++)
    {
        delete _textures[i];
    }
    
    _textures.clear();
}

void TextureCollection::removeTextureAtIndex(uint index, ItemCollection &items)
{
    Texture *texture = _textures.at(index);
    texture->removeFromItems(items);
    _textures.erase(_textures.begin() + index);
    delete texture;
}

uint TextureCollection::indexOfTexture(Texture *texture)
{
    for (uint i = 0; i < _textures.size(); i++)
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
        uint textureCount = stream->read<uint>();
        for (uint i = 0; i < textureCount; i++)
        {
            uint charCount = stream->read<uint>();
            char *utf8String = (char *)malloc(charCount + 1);
            memset(utf8String, 0, charCount + 1);
            stream->readBytes(utf8String, charCount);
#if defined(__APPLE__)
            NSString *name = [NSString stringWithUTF8String:utf8String];
#elif defined(WIN32)
			String ^name = gcnew String(utf8String);
#elif defined(__linux__)
#warning "TextureCollection(stream), name"
#endif
            free(utf8String);
            Texture *texture = new Texture();
            texture->setName(name);

#if defined(__APPLE__)
            // image will be opened if it is in Pictures folder or inside .folder3D
            // there is currently no security scoped bookmarks to image file
            NSImage *image = [[NSImage alloc] initWithContentsOfFile:name];
#elif defined(WIN32)
			Bitmap ^image = nullptr;
			if (File::Exists(name))
				image = gcnew Bitmap(name);
#elif defined(__linux__)
#warning "TextureCollection(stream), image"
#endif
			if (image)
                texture->setImage(image);
            
            addTexture(texture);
        }
    }
}

void TextureCollection::encode(MemoryWriteStream *stream)
{
    uint textureCount = _textures.size();
    stream->write<uint>(textureCount);
    for (uint i = 0; i < textureCount; i++)
    {
#if defined(__APPLE__)
        NSString *name = _textures[i]->name();
        
        uint charCount = [name lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
        const char *utf8String = [name UTF8String];		
#elif defined(WIN32)
		String ^name = _textures[i]->name();

		array<Byte> ^bytes = System::Text::Encoding::UTF8->GetBytes(name);
		uint charCount = bytes->Length;
		pin_ptr<Byte> utf8String = &bytes[0];		
#elif defined(__linux__)
#warning "TextureCollection::encode(stream)"
#endif  
		stream->write<uint>(charCount);
        stream->writeBytes(utf8String, charCount);
    }
}
