//
//  ShaderProgram.m
//  OpenGLEditor
//
//  Created by Filip Kunc on 12/31/09.
//  For license see LICENSE.TXT
//

#import "ShaderProgram.h"

static inline const char * GetGLErrorString(GLenum error)
{
	const char *str;
	switch( error )
	{
		case GL_NO_ERROR:
			str = "GL_NO_ERROR";
			break;
		case GL_INVALID_ENUM:
			str = "GL_INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			str = "GL_INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			str = "GL_INVALID_OPERATION";
			break;
#if defined __gl_h_ || defined __gl3_h_
		case GL_OUT_OF_MEMORY:
			str = "GL_OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			str = "GL_INVALID_FRAMEBUFFER_OPERATION";
			break;
#endif
#if defined __gl_h_
		case GL_STACK_OVERFLOW:
			str = "GL_STACK_OVERFLOW";
			break;
		case GL_STACK_UNDERFLOW:
			str = "GL_STACK_UNDERFLOW";
			break;
		case GL_TABLE_TOO_LARGE:
			str = "GL_TABLE_TOO_LARGE";
			break;
#endif
		default:
			str = "(ERROR: Unknown Error Enum)";
			break;
	}
	return str;
}

#define GetGLError()									\
{														\
	GLenum err = glGetError();							\
	while (err != GL_NO_ERROR) {						\
		NSLog(@"GLError %s set in File:%s Line:%d\n",	\
				GetGLErrorString(err),					\
				__FILE__,								\
				__LINE__);								\
		err = glGetError();								\
	}													\
}

ShaderProgram *globalNormalShader = nil;

@implementation ShaderProgram

+ (void)initShaders
{
    [[ShaderProgram normalShader] useProgram];
}

+ (ShaderProgram *)normalShader
{
	if (!globalNormalShader)
	{
		globalNormalShader = [[ShaderProgram alloc] init];
		[globalNormalShader attachShaderWithType:GL_VERTEX_SHADER resourceInBundle:@"vertex"];
        [globalNormalShader attachShaderWithType:GL_FRAGMENT_SHADER resourceInBundle:@"fragment"];
        [globalNormalShader linkProgram];
	}
	return globalNormalShader;
}

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
}

- (void)attachShaderWithType:(GLenum)type resourceInBundle:(NSString *)resourceInBundle
{
	Shader *shader = [[Shader alloc] initWithShaderType:type resourceInBundle:resourceInBundle];
	[self attachShader:shader]; // performs shader release
}

- (void)attachShader:(Shader *)aShader
{
	glAttachShader(program, [aShader shader]);
}

// GL_TRIANGLES, GL_TRIANGLE_STRIP
- (void)setGeometryInput:(GLenum)input output:(GLenum)output
{
	glProgramParameteriEXT(program, GL_GEOMETRY_INPUT_TYPE_EXT, (int)input);
	glProgramParameteriEXT(program, GL_GEOMETRY_OUTPUT_TYPE_EXT, (int)output);
	
	int temp;
	glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT, &temp);
	glProgramParameteriEXT(program, GL_GEOMETRY_VERTICES_OUT_EXT, temp);
}

- (void)linkProgram
{
	glLinkProgram(program);	
	GLint logLength, status;
    
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0)
	{
		GLchar *log = (GLchar*)malloc(logLength);
		glGetProgramInfoLog(program, logLength, &logLength, log);
		NSLog(@"Program link log:\n%s\n", log);
		free(log);
	}
	
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == 0)
	{
		NSLog(@"Failed to link program");
		return;
	}
	
//	glValidateProgram(program);
//	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
//	if (logLength > 0)
//	{
//		GLchar *log = (GLchar*)malloc(logLength);
//		glGetProgramInfoLog(program, logLength, &logLength, log);
//		NSLog(@"Program validate log:\n%s\n", log);
//		free(log);
//	}
//	
//	glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
//	if (status == 0)
//	{
//		NSLog(@"Failed to validate program");
//		return;
//	}
    
    glUseProgram(program);
    
    GetGLError();
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
