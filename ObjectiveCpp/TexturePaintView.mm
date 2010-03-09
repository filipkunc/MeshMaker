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

@synthesize delegate;

- (id)initWithFrame:(NSRect)frame 
{
    self = [super initWithFrame:frame];
    if (self) 
	{
        // Initialization code here.
		textureObjectID = 0;
		textureWidth = 512;
		textureHeight = 512;
		initialized = NO;
		
		lastPoint = currentPoint = NSZeroPoint;
		drawing = NO;
		
		changed = NO;
		
		canvas = [[NSImage alloc] initWithSize:NSMakeSize(textureWidth, textureHeight)];
		[canvas lockFocus];
		[[NSColor whiteColor] setFill];
		NSRectFill(NSMakeRect(0, 0, textureWidth, textureHeight));
		[canvas unlockFocus];
	}
    return self;
}

- (void)dealloc
{
	[canvas release];
	[super dealloc];
}

- (void)drawRect:(NSRect)dirtyRect 
{
	[canvas drawInRect:[self bounds]
			  fromRect:NSMakeRect(0, 0, [canvas size].width, [canvas size].height)
			 operation:NSCompositeSourceOver
			  fraction:1.0];
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
	
	[canvas lockFocus];
	
	[[NSColor colorWithCalibratedRed:0.5f green:0.3f blue:1.0f alpha:0.3f] setStroke];

	NSBezierPath *bezierPath = [NSBezierPath bezierPath];
	[bezierPath setLineWidth:50.0f];
	[bezierPath setLineCapStyle:NSRoundLineCapStyle];
	
	[bezierPath moveToPoint:lastPoint];
	[bezierPath lineToPoint:currentPoint];
	
	[bezierPath stroke];
	
	[canvas unlockFocus];
	changed = YES;
	
	lastPoint = currentPoint;
	
	[self setNeedsDisplay:YES];
	
	[delegate canvasHasChanged];
}

- (void)mouseUp:(NSEvent *)e
{
	drawing = NO;
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
	
	initialized = YES;
	changed = YES;
}

- (void)updateTexture
{
	if (!initialized)
		[self initTexture];
	
	if (!changed)
		return;
	
	NSBitmapImageRep *bitmap = [NSBitmapImageRep imageRepWithData:[canvas TIFFRepresentation]];
		
	glBindTexture(GL_TEXTURE_2D, textureObjectID);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, [bitmap bitmapData]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	changed = NO;
}

@end
