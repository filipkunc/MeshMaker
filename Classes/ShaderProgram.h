//
//  ShaderProgram.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 12/31/09.
//  For license see LICENSE.TXT
//

#import <Cocoa/Cocoa.h>
#import "Shader.h"

@interface ShaderProgram : NSObject
{
	GLuint program;
}

- (void)attachShaderWithType:(GLenum)type resourceInBundle:(NSString *)resourceInBundle;
- (void)attachShader:(Shader *)aShader; // attachShader sends release to aShader object
- (void)setGeometryInput:(GLenum)input output:(GLenum)output;
- (void)linkProgram;
- (void)useProgram;
+ (void)resetProgram;

@end
