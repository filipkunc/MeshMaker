//
//  FPTexture.h
//  SpriteLib
//
//  Created by Filip Kunc on 4/12/10.
//  For license see LICENSE.TXT
//

#pragma once

#include "OpenGLDrawing.h"

void CreateTexture(GLubyte *data, int components, GLuint *textureID, int width, int height, bool convertToAlpha);

class ItemCollection;

class Texture
{
private:
    GLuint _textureID;
    __strong NSString *_name;
    __strong NSImage *_image;
    bool _needUpdate;
public:
    Texture();
    Texture(NSString *name, NSImage *image, bool convertToAlpha = false);
    ~Texture();
    
    // for ARC
    Texture(const Texture& other);
    Texture &operator=(const Texture &other);
    
    GLuint textureID() { return _textureID; }
    NSString *name() { return _name; }
    void setName(NSString *name) { _name = [name copy]; }
    NSImage *image() { return _image; }
    void setImage(NSImage *image);
    
    void drawForUnwrap();
    void updateTexture();
    
    void removeFromItems(ItemCollection &items);
};
