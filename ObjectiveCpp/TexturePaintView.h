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

@protocol TexturePaintViewDelegate

- (void)canvasHasChanged;

@end


@interface TexturePaintView : NSView 
{
@public
	GLuint textureObjectID;	
	GLuint textureWidth;
	GLuint textureHeight;
	BOOL initialized;
	
	NSPoint lastPoint;
	NSPoint currentPoint;
	BOOL drawing;
	
	BOOL changed;
	
	NSImage *canvas;
	
	IBOutlet id<TexturePaintViewDelegate> delegate;
}

@property (readwrite, assign) id<TexturePaintViewDelegate> delegate;

- (NSRect)currentRect;
- (void)initTexture;
- (void)updateTexture;

@end
