//
//  Shader.m
//  OpenGLEditor
//
//  Created by Filip Kunc on 12/26/09.
//  For license see LICENSE.TXT
//

#import "Shader.h"

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
        
        GLint logLength, status;
        
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0)
        {
            GLchar *log = (GLchar*)malloc(logLength);
            glGetShaderInfoLog(shader, logLength, &logLength, log);
            NSLog(@"%@ compile log:\n%s\n", path, log);
            free(log);
        }
        
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status == 0)
        {
            NSLog(@"Failed to compile %@ shader:\n%s\n", path, shaderSource);
            return 0;
        }
	}
	return self;	
}

- (void)dealloc
{
	glDeleteShader(shader);
}

@end


