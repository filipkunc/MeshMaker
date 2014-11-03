//
//  ShaderProgram.m
//  OpenGLEditor
//
//  Created by Filip Kunc on 12/31/09.
//  For license see LICENSE.TXT
//

#include "ShaderProgram.h"

#include <stdio.h>

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
