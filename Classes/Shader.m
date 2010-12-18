//
//  Shader.m
//  OpenGLEditor
//
//  Created by Filip Kunc on 12/26/09.
//  For license see LICENSE.TXT
//

#import "Shader.h"

// modified from http://www.lighthouse3d.com/opengl/glsl/index.php?oglinfo

void ShaderLog(GLuint shader)
{
	int infologLength = 0;
	int charsWritten = 0;
	GLchar *infoLog;
	
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);
	
	if (infologLength > 0)
	{
		infoLog = (GLchar *)malloc(infologLength);
		glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
		NSLog(@"shader log: %s",infoLog);
		free(infoLog);
	}
}

@implementation Shader

@synthesize shader, type;

+ (NSString *)fileExtensionForShaderType:(GLenum)aShaderType
{
	switch (aShaderType)
	{
		case GL_VERTEX_SHADER:
			return @"vs";
		case GL_FRAGMENT_SHADER:
			return @"fs";
		case GL_GEOMETRY_SHADER_EXT:
			return @"gs";
		default:
			return nil;
	}
}

- (id)initWithShaderType:(GLenum)aShaderType
		resourceInBundle:(NSString *)resourceInBundle
{
	self = [super init];
	if (self)
	{
		type = aShaderType;
		NSBundle *bundle = [NSBundle mainBundle];
		NSString *extension = [Shader fileExtensionForShaderType:aShaderType];
		NSString *path = [bundle pathForResource:resourceInBundle ofType:extension];
		
		NSString *contents;
		contents = [NSString stringWithContentsOfFile:path
											 encoding:NSUTF8StringEncoding
												error:NULL];
		
		const GLchar *shaderSource = [contents cStringUsingEncoding:NSASCIIStringEncoding];
		
		shader = glCreateShader(type);
		glShaderSource(shader, 1, &shaderSource, NULL);
		glCompileShader(shader);
		ShaderLog(shader);
	}
	return self;	
}

- (void)dealloc
{
	glDeleteShader(shader);
	[super dealloc];
}

@end


