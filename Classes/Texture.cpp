//
//  FPTexture.mm
//  SpriteLib
//
//  Created by Filip Kunc on 4/12/10.
//  For license see LICENSE.TXT
//

#include "Texture.h"
#include "ItemCollection.h"

void CreateTexture(GLubyte *data, int components, GLuint *textureID, int width, int height, bool convertToAlpha)
{
	glEnable(GL_TEXTURE_2D);
	if (*textureID == 0)
		glGenTextures(1, textureID);
	glBindTexture(GL_TEXTURE_2D, *textureID);
	
	if (convertToAlpha)
	{
		GLubyte *alphaData = (GLubyte *)malloc((size_t)(width * height));
		for (int i = 0; i < width * height; i++)
			alphaData[i] = data[i * components];
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, alphaData);
		free(alphaData);
	}
	else 
	{
		if (components == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if (components == 4)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		else
			throw MeshMaker::UnsupportedImageFormatException();
	}
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

NSBitmapImageRep *BitmapImageRepFromImage(NSImage *image);

NSBitmapImageRep *BitmapImageRepFromImage(NSImage *image)
{
    NSImage *copy = [image copy];
    NSBitmapImageRep* bitmap = [NSBitmapImageRep alloc];
    NSSize imgSize = [copy size];
    
    [copy setFlipped:YES];
    [copy lockFocus];
    bitmap = [bitmap initWithFocusedViewRect:NSMakeRect(0.0, 0.0, imgSize.width, imgSize.height)];
    [copy unlockFocus];
    
    return bitmap;
}

Texture::Texture()
{
    _textureID = 0;
	_name = nullptr;
    _image = nullptr;
    _needUpdate = false;    
}

Texture::Texture(NSString *name, NSImage *image, bool convertToAlpha)
{
    _name = [name copy];
    _image = [image copy];
    
    NSBitmapImageRep *bitmap = BitmapImageRepFromImage(_image);
    GLubyte *data = [bitmap bitmapData];
    int bitsPerPixel = static_cast<int>([bitmap bitsPerPixel]);
    int components = bitsPerPixel / 8;
    
    _needUpdate = false;
    
    CreateTexture(data, components, &_textureID, _image.size.width, _image.size.height, convertToAlpha);
}

Texture::~Texture()
{
    if (_textureID > 0U)
        glDeleteTextures(1, &_textureID);
}

Texture::Texture(const Texture& other)
{
    _textureID = other._textureID;
    _image = [other._image copy];
    _needUpdate = other._needUpdate;
}

Texture &Texture::operator=(const Texture &other)
{
    _textureID = other._textureID;
    _image = [other._image copy];
    _needUpdate = other._needUpdate;
    return *this;
}

void Texture::setImage(NSImage *image)
{
    _image = [image copy];
	_needUpdate = true;
}

struct TexturedVertex2D
{
	float x, y;
	float s, t;
};

void Texture::drawForUnwrap()
{
    const TexturedVertex2D vertices[] = 
	{
		{ 0, 0, 0, 0, }, // 0
		{ 1, 0, 1, 0, }, // 1
		{ 0, 1,	0, 1, }, // 2
		{ 1, 1,	1, 1, }, // 3
	};
    
    glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _textureID);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, sizeof(TexturedVertex2D), &vertices->x);	
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, sizeof(TexturedVertex2D), &vertices->s);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_TEXTURE_2D);
}

void Texture::updateTexture()
{
    if (!_needUpdate)
        return;

    NSBitmapImageRep *bitmap = BitmapImageRepFromImage(_image);

    GLubyte *data = [bitmap bitmapData];
    int bitsPerPixel = static_cast<int>([bitmap bitsPerPixel]);
	int components = bitsPerPixel / 8;
        
	CreateTexture(data, components, &_textureID, _image.size.width, _image.size.height, NO);  

    _needUpdate = false;
}

void Texture::removeFromItems(ItemCollection &items)
{
    for (uint i = 0; i < items.count(); i++)
    {
        Item *item = items.itemAtIndex(i);
        if (item->mesh->texture() == this)
            item->mesh->setTexture(NULL);
    }
}