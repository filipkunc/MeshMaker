//
//  FPTexture.mm
//  SpriteLib
//
//  Created by Filip Kunc on 4/12/10.
//  For license see LICENSE.TXT
//

#import "FPTexture.h"

void CreateTexture(GLubyte *data, int components, GLuint *textureID, int width, int height, BOOL convertToAlpha)
{
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, textureID);
	glBindTexture(GL_TEXTURE_2D, *textureID);
	
	if (convertToAlpha)
	{
		GLubyte *alphaData = (GLubyte *)malloc(width * height);
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
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			@throw [NSException exceptionWithName:@"Unsupported image format" reason:nil userInfo:nil];

	}
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

@implementation FPTexture

@synthesize textureID, needUpdate;

- (int)width
{
    return (int)_image.size.width;
}

- (int)height
{
    return (int)_image.size.height;
}

- (NSImage *)canvas
{
    return _image;
}

- (void)setCanvas:(NSImage *)canvas
{
    _image = [canvas copy];
    needUpdate = YES;
}

- (id)init
{
    self = [super init];
    if (self)
    {
        _image = nil;
        textureID = 0;
        needUpdate = NO;
    }
    return self;        
}

- (id)initWithImage:(NSImage *)image convertToAlpha:(BOOL)convertToAlpha
{
    self = [super init];
	if (self)
	{
        _image = [image copy];
		
		NSBitmapImageRep *bitmap = [self bitmapImageRepFromImage:image];
		GLubyte *data = [bitmap bitmapData];
		NSInteger bitsPerPixel = [bitmap bitsPerPixel];
		int components = bitsPerPixel / 8;
        
        needUpdate = NO;
        
		CreateTexture(data, components, &textureID, _image.size.width, _image.size.height, convertToAlpha);
		return self;
	}
	return nil;	
}

- (id)initWithFile:(NSString *)fileName convertToAlpha:(BOOL)convertToAlpha
{
    return [self initWithImage:[NSImage imageNamed:fileName] convertToAlpha:convertToAlpha];
}

- (void)dealloc
{
    if (textureID > 0U)
        glDeleteTextures(1, &textureID);
}

struct FPVertex
{
	float x, y;
	float s, t;
};

- (void)drawForUnwrap
{
    const FPVertex vertices[] = 
	{
		{ 0, 0, 0, 0, }, // 0
		{ 1, 0, 1, 0, }, // 1
		{ 0, 1,	0, 1, }, // 2
		{ 1, 1,	1, 1, }, // 3
	};
    
    glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, sizeof(FPVertex), &vertices->x);	
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, sizeof(FPVertex), &vertices->s);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_TEXTURE_2D);
}

- (void)drawAtPoint:(CGPoint)pt
{
    float x = (float)pt.x;
    float y = (float)pt.y;
    float width = self.width;
    float height = self.height;
    
    const FPVertex vertices[] = 
	{
		{ x,         y,             0, 0, }, // 0
		{ x + width, y,             1, 0, }, // 1
		{ x,		 y - height,	0, 1, }, // 2
		{ x + width, y - height, 	1, 1, }, // 3
	};
    
    glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, sizeof(FPVertex), &vertices->x);	
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, sizeof(FPVertex), &vertices->s);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_TEXTURE_2D);
}

- (void)updateTexture
{
    NSBitmapImageRep *bitmap = [self bitmapImageRepFromImage:_image];

    if (textureID == 0)
    {
        GLubyte *data = [bitmap bitmapData];
		NSInteger bitsPerPixel = [bitmap bitsPerPixel];
		int components = bitsPerPixel / 8;
        
		CreateTexture(data, components, &textureID, self.width, self.height, NO);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, 4, self.width, self.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, [bitmap bitmapData]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);        
    }
    
    needUpdate = NO;
}

- (NSBitmapImageRep *)bitmapImageRepFromImage:(NSImage *)theImg
{
    NSImage *copy = [theImg copy];
    NSBitmapImageRep* bitmap = [NSBitmapImageRep alloc];
    NSSize imgSize = [copy size];    
    
    [copy setFlipped:YES];
    [copy lockFocus];
    bitmap = [bitmap initWithFocusedViewRect:NSMakeRect(0.0, 0.0, imgSize.width, imgSize.height)];
    [copy unlockFocus];
    
    return bitmap;
}

+ (void)drawString:(NSString *)string atPoint:(CGPoint)point
{
    CGSize frameSize = CGSizeMake(256.0f, 128.0f);
    NSImage *image = [[NSImage alloc] initWithSize:frameSize];

    [image lockFocus];        
    [[NSColor blackColor] set];
    [string drawAtPoint:NSMakePoint(0.0f, 0.0f) withAttributes:nil];
    [image unlockFocus];
    
    FPTexture *stringTexture = [[FPTexture alloc] initWithImage:image convertToAlpha:NO];    
    [stringTexture drawAtPoint:point];
}

@end

