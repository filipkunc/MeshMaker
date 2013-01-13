//
//  Shader.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 12/26/09.
//  For license see LICENSE.TXT
//

#pragma once

#if defined(__APPLE__) || defined(SHADERS)

#include "OpenGLDrawing.h"

class Shader
{
public:
    GLuint shader;
    GLenum type;
    
    Shader(GLenum shaderType, const char *fileName);
    ~Shader();
};

#endif