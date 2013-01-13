//
//  ShaderProgram.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 12/31/09.
//  For license see LICENSE.TXT
//

#pragma once

#if defined(__APPLE__) || defined(SHADERS)

#include "Shader.h"

class ShaderProgram
{
public:
    GLuint program;
    
    ShaderProgram();
    ~ShaderProgram();
    
    static void initShaders();
    static ShaderProgram *normalShader();
    void attachShader(GLenum type, const char *fileName);
    void linkProgram();
    void useProgram();
    static void resetProgram();
};

#endif