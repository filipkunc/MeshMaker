//
//  ShaderProgram.m
//  OpenGLEditor
//
//  Created by Filip Kunc on 12/31/09.
//  For license see LICENSE.TXT
//

#include "ShaderProgram.h"

#if defined(__APPLE__) || defined(SHADERS)

#include <stdio.h>

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
		printf("GLError %s set in File:%s Line:%d\n",	\
				GetGLErrorString(err),					\
				__FILE__,								\
				__LINE__);								\
		err = glGetError();								\
	}													\
}

ShaderProgram *globalNormalShader = NULL;
ShaderProgram *globalTexturedShader = NULL;

void ShaderProgram::initShaders()
{
    ShaderProgram::normalShader()->useProgram();
    ShaderProgram::texturedShader()->useProgram();
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

ShaderProgram *ShaderProgram::texturedShader()
{
    if (!globalTexturedShader)
	{
		globalTexturedShader = new ShaderProgram();
        globalTexturedShader->attachShader(GL_VERTEX_SHADER, "vertex.vs");
        globalTexturedShader->attachShader(GL_FRAGMENT_SHADER, "textured_frag.fs");
        globalTexturedShader->linkProgram();
	}
	return globalTexturedShader;
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
        printf("Program link log:\n%s\n", log);
		free(log);
	}
	
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == 0)
	{
        printf("Failed to link program");
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

#endif
