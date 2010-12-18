//
//  ShaderProgram.m
//  OpenGLEditor
//
//  Created by Filip Kunc on 12/31/09.
//  For license see LICENSE.TXT
//

#import "ShaderProgram.h"

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

- (void)attachShaderWithType:(GLenum)type resourceInBundle:(NSString *)resourceInBundle
{
	Shader *shader = [[Shader alloc] initWithShaderType:type resourceInBundle:resourceInBundle];
	[self attachShader:shader]; // performs shader release
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

+ (void)resetProgram
{
	glUseProgram(0);
}

@end
