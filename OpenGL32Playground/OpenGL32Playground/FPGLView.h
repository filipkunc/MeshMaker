//
//  FPGLView.h
//  OpenGL32Playground
//
//  Created by Filip Kunc on 7/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <OpenGL/gl3.h>

@interface FPGLView : NSOpenGLView
{
    GLuint _shaderProgram;
    GLuint _vertexShader;
    GLuint _fragmentShader;
    GLuint _vertexArray;
    GLuint _vertexBuffer;
    GLuint _mvpLocation;
}

@end
