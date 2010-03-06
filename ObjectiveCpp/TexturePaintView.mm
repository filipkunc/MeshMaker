//
//  TexturePaintView.m
//  OpenGLEditor
//
//  Created by Filip Kunc on 2/28/10.
//  For license see LICENSE.TXT
//

#import "TexturePaintView.h"
#import "MathDeclaration.h"

@implementation TexturePaintView

- (id)initWithFrame:(NSRect)frame 
{
    self = [super initWithFrame:frame];
    if (self) 
	{
        // Initialization code here.
		frameBufferObjectID = 0;
		textureObjectID = 0;
		textureWidth = 512;
		textureHeight = 512;
		initialized = NO;
		
		lastPoint = currentPoint = NSZeroPoint;
		drawing = NO;
		rectangles = [[NSMutableArray alloc] init];
    }
    return self;
}

- (void)drawRect:(NSRect)dirtyRect 
{
    // Drawing code here.
	[[NSColor whiteColor] setFill];
	[[NSColor blackColor] setStroke];
	
	for (NSValue *value in rectangles)
	{
		[[NSBezierPath bezierPathWithRoundedRect:[value rectValue]
										 xRadius:4.0f
										 yRadius:4.0f] stroke];
	}
	
	if (drawing)
	{
		[[NSColor blueColor] setStroke];
		[[NSBezierPath bezierPathWithRoundedRect:[self currentRect]
										 xRadius:4.0f
										 yRadius:4.0f] stroke];
	}
}

- (NSPoint)locationFromNSEvent:(NSEvent *)e
{
	return [self convertPoint:[e locationInWindow] fromView:nil];
}

- (NSRect)currentRect
{
	float minX = Min(lastPoint.x, currentPoint.x);
	float maxX = Max(lastPoint.x, currentPoint.x);
	float minY = Min(lastPoint.y, currentPoint.y);
	float maxY = Max(lastPoint.y, currentPoint.y);
	
	return NSMakeRect(minX, minY, maxX - minX, maxY - minY);
}

- (void)mouseDown:(NSEvent *)e
{
	lastPoint = currentPoint = [self locationFromNSEvent:e];
	drawing = YES;
}

- (void)mouseDragged:(NSEvent *)e
{
	currentPoint = [self locationFromNSEvent:e];
	[self setNeedsDisplay:YES];
}

- (void)mouseUp:(NSEvent *)e
{
	drawing = NO;
	[rectangles addObject:[NSValue valueWithRect:[self currentRect]]];
 	[self setNeedsDisplay:YES];						   
}

- (void)initTexture
{
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &textureObjectID);
	glBindTexture(GL_TEXTURE_2D, textureObjectID);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenFramebuffersEXT(1, &frameBufferObjectID);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBufferObjectID);
	
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, textureObjectID, 0);
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	
	initialized = YES;
}

- (void)renderStuff
{
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);		
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();			
	glOrtho(0, textureWidth, 0, textureHeight, -1000.0f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glColor3f(0, 0.5f, 0.5f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glRecti(10, 10, 200, 50);
	
	glFlush();
}

- (void)updateTexture
{
	if (!initialized)
		[self initTexture];
	
	//glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBufferObjectID);
//	glViewport(0, 0, textureWidth, textureHeight);
	
//	[self renderStuff];
	
	NSImage *screenshot = [[[NSImage alloc] initWithSize:NSMakeSize(textureWidth, textureHeight)] autorelease];
	[screenshot lockFocus];
	[[NSColor whiteColor] setFill];
	NSRectFill(NSMakeRect(0, 0, textureWidth, textureHeight));
	[self drawRect: [self frame]];
	[screenshot unlockFocus];
	
	NSBitmapImageRep *bitmap = [NSBitmapImageRep imageRepWithData:[screenshot TIFFRepresentation]];
		
	glBindTexture(GL_TEXTURE_2D, textureObjectID);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, [bitmap bitmapData]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	//glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

@end
