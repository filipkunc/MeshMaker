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

void ProgramLog(GLuint program)
{
	int infologLength = 0;
	int charsWritten = 0;
	GLchar *infoLog;
	
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLength);
	
	if (infologLength > 0)
	{
		infoLog = (GLchar *)malloc(infologLength);
		glGetProgramInfoLog(program, infologLength, &charsWritten, infoLog);
		NSLog(@"program log: %s", infoLog);
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

static ShaderProgram *currentShaderProgram;

@implementation ShaderProgram

- (id)init
{
	self = [super init];
	if (self)
	{
		program = glCreateProgram();
	}
	return self;
}

- (void)dealloc
{
	glDeleteProgram(program);
	[super dealloc];
}

- (void)attachShader:(Shader *)aShader
{
	glAttachShader(program, [aShader shader]);
	[aShader release]; // shader is not needed after this
}

// GL_TRIANGLES, GL_TRIANGLE_STRIP
- (void)setGeometryInput:(GLenum)input output:(GLenum)output
{
	glProgramParameteriEXT(program, GL_GEOMETRY_INPUT_TYPE_EXT, input);
	glProgramParameteriEXT(program, GL_GEOMETRY_OUTPUT_TYPE_EXT, output);
	
	int temp;
	glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT, &temp);
	glProgramParameteriEXT(program, GL_GEOMETRY_VERTICES_OUT_EXT, temp);
}

- (void)linkProgram
{
	glLinkProgram(program);	
	ProgramLog(program);
}

- (void)useProgram
{
	glUseProgram(program);
}

+ (void)setCurrentShaderProgram:(ShaderProgram *)shaderProgram
{
	currentShaderProgram = shaderProgram;
}

+ (ShaderProgram *)currentShaderProgram
{
	return currentShaderProgram;
}

+ (void)resetProgram
{
	glUseProgram(0);
}

@end

