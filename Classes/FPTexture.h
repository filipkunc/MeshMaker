//
//  FPTexture.h
//  SpriteLib
//
//  Created by Filip Kunc on 4/12/10.
//  For license see LICENSE.TXT
//

#import "OpenGLDrawing.h"

void CreateTexture(GLubyte *data, int components, GLuint *textureID, int width, int height, BOOL convertToAlpha);

@interface FPTexture : NSObject 
{
    NSImage *_image;
	GLuint textureID;
	int width;
	int height;
}

@property (readonly, assign) GLuint textureID;
@property (readonly, assign) int width;
@property (readonly, assign) int height;
@property (readonly) NSImage *canvas;

- (id)initWithImage:(NSImage *)image convertToAlpha:(BOOL)convertToAlpha;
- (id)initWithFile:(NSString *)fileName convertToAlpha:(BOOL)convertToAlpha;
- (void)drawAtPoint:(CGPoint)point;
- (void)drawForUnwrap;
- (void)updateTexture;
+ (void)drawString:(NSString *)string atPoint:(CGPoint)point;

@end
