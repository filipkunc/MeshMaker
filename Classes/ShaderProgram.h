//
//  ShaderProgram.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 12/31/09.
//  For license see LICENSE.TXT
//

#pragma once

#include "Shader.h"

#if defined(__APPLE__) || defined(SHADERS)

class ShaderProgram
{
public:
    GLuint program;
    
    ShaderProgram();
    ~ShaderProgram();
    
    static void initShaders();
    static ShaderProgram *normalShader();
    static ShaderProgram *texturedShader();
    
    void attachShader(GLenum type, const char *fileName);
    void linkProgram();
    void useProgram();
    static void resetProgram();
};

#endif
