//
//  Shader.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 12/26/09.
//  For license see LICENSE.TXT
//

#pragma once

#include "OpenGLDrawing.h"

#if defined(__APPLE__) || defined(SHADERS)

class Shader
{
public:
    GLuint shader;
    GLenum type;
    
    Shader(GLenum shaderType, const char *fileName);
    ~Shader();
};

#endif
