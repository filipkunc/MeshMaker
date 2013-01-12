//
//  ShaderProgram.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 12/31/09.
//  For license see LICENSE.TXT
//

#pragma once

#import <Cocoa/Cocoa.h>
#import "Shader.h"

@interface ShaderProgram : NSObject
{
	GLuint program;
}

@property (readonly) GLuint program;

+ (void)initShaders;
+ (ShaderProgram *)normalShader;
- (void)attachShaderWithType:(GLenum)type resourceInBundle:(NSString *)resourceInBundle;
- (void)attachShader:(Shader *)aShader; // attachShader sends release to aShader object
- (void)setGeometryInput:(GLenum)input output:(GLenum)output;
- (void)linkProgram;
- (void)useProgram;
+ (void)resetProgram;

@end
