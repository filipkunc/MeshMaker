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

@synthesize textureID, width, height;

- (id)initWithImage:(NSImage *)image convertToAlpha:(BOOL)convertToAlpha
{
    self = [super init];
	if (self)
	{
		width = [image size].width;
		height = [image size].height;
		
		NSBitmapImageRep *bitmap = [NSBitmapImageRep imageRepWithData:[image TIFFRepresentation]];
		GLubyte *data = [bitmap bitmapData];
		NSInteger bitsPerPixel = [bitmap bitsPerPixel];
		int components = bitsPerPixel / 8;
        
		CreateTexture(data, components, &textureID, width, height, convertToAlpha);
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
    glDeleteTextures(1, &textureID);
}

struct FPVertex
{
	float x, y;
	float s, t;
};

- (void)drawAtPoint:(CGPoint)pt
{
    float x = (float)pt.x;
    float y = (float)pt.y;
    
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

