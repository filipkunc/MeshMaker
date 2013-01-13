//
//  Shader.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 12/26/09.
//  For license see LICENSE.TXT
//

#include "Shader.h"

Shader::Shader(GLenum shaderType, const char *fileName)
{
    type = shaderType;
    
#if defined(__APPLE__)
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
#elif defined(WIN32)
    string buffer;
    ifstream fin(fileName);
    getline(fin, buffer, char(-1));
    fin.close();
    const GLchar *shaderSource = buffer.c_str();
#endif
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
    }
}

Shader::~Shader()
{
    glDeleteShader(shader);
}
