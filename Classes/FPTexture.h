//
//  FPTexture.h
//  SpriteLib
//
//  Created by Filip Kunc on 4/12/10.
//  For license see LICENSE.TXT
//

#pragma once

#import "OpenGLDrawing.h"

void CreateTexture(GLubyte *data, int components, GLuint *textureID, int width, int height, BOOL convertToAlpha);

@interface FPTexture : NSObject 
{
    NSImage *_image;
	GLuint textureID;
    BOOL needUpdate;
}

@property (readonly, assign) GLuint textureID;
@property (readonly, assign) int width;
@property (readonly, assign) int height;
@property (readwrite, copy) NSImage *canvas;
@property (readwrite, assign) BOOL needUpdate;

- (id)initWithImage:(NSImage *)image convertToAlpha:(BOOL)convertToAlpha;
- (id)initWithFile:(NSString *)fileName convertToAlpha:(BOOL)convertToAlpha;
- (void)drawAtPoint:(CGPoint)point;
- (void)drawForUnwrap;
- (void)updateTexture;
- (NSBitmapImageRep *)bitmapImageRepFromImage:(NSImage *)theImg;
+ (void)drawString:(NSString *)string atPoint:(CGPoint)point;

@end
