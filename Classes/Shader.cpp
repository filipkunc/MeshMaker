//
//  Shader.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 12/26/09.
//  For license see LICENSE.TXT
//

#include "Shader.h"
#include <stdio.h>

Shader::Shader(GLenum shaderType, const char *fileName)
{
    type = shaderType;
    
    NSBundle *bundle = [NSBundle mainBundle];
    NSString *file = [NSString stringWithUTF8String:fileName];
    NSString *extension = [file pathExtension];
    NSString *resourceInBundle = [file stringByDeletingPathExtension];
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
		printf("%s compile log:\n%s\n", fileName, log);
        free(log);
    }
    
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == 0)
    {
		printf("Failed to compile %s shader:\n%s\n", fileName, shaderSource);
    }
}

Shader::~Shader()
{
    glDeleteShader(shader);
}
