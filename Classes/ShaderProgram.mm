//
//  ShaderProgram.m
//  OpenGLEditor
//
//  Created by Filip Kunc on 12/31/09.
//  For license see LICENSE.TXT
//

#include "ShaderProgram.h"

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

ShaderProgram *globalNormalShader = NULL;

void ShaderProgram::initShaders()
{
    ShaderProgram::normalShader()->useProgram();
}

ShaderProgram *ShaderProgram::normalShader()
{
    if (!globalNormalShader)
	{
		globalNormalShader = new ShaderProgram();
        globalNormalShader->attachShader(GL_VERTEX_SHADER, "vertex.vs");
        globalNormalShader->attachShader(GL_FRAGMENT_SHADER, "fragment.fs");
        globalNormalShader->linkProgram();
	}
	return globalNormalShader;
}

ShaderProgram::ShaderProgram()
{
    program = glCreateProgram();
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(program);
}

void ShaderProgram::attachShader(GLenum type, const char *fileName)
{
    Shader *shader = new Shader(type, fileName);
    glAttachShader(program, shader->shader);
    delete shader;
}

void ShaderProgram::linkProgram()
{
	glLinkProgram(program);	
	GLint logLength, status;
    
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0)
	{
		GLchar *log = (GLchar*)malloc(logLength);
		glGetProgramInfoLog(program, logLength, &logLength, log);
#if defined(__APPLE__)
		NSLog(@"Program link log:\n%s\n", log);
#elif defined(WIN32)
        printf("Program link log:\n%s\n", log);
#endif
		free(log);
	}
	
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == 0)
	{
#if defined(__APPLE__)
		NSLog(@"Failed to link program");
#elif defined(WIN32)
        printf("Failed to link program");
#endif
		return;
	}
	
    GetGLError();
}

void ShaderProgram::useProgram()
{
    glUseProgram(program);
}

void ShaderProgram::resetProgram()
{
    glUseProgram(0);
}
