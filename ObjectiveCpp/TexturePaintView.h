//
//  TexturePaintView.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 2/28/10.
//  For license see LICENSE.TXT
//

#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>
#import <OpenGL/glu.h>

@interface TexturePaintView : NSView 
{
@public
	GLuint frameBufferObjectID;
	GLuint textureObjectID;	
	GLuint textureWidth;
	GLuint textureHeight;
	BOOL initialized;
	
	NSPoint lastPoint;
	NSPoint currentPoint;
	BOOL drawing;
	
	NSMutableArray *rectangles;
}

- (NSRect)currentRect;
- (void)initTexture;
- (void)renderStuff;
- (void)updateTexture;

@end
