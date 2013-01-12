//
//  Shader.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 12/26/09.
//  For license see LICENSE.TXT
//

#pragma once

#import "OpenGLDrawing.h"

@interface Shader : NSObject 
{
	GLuint shader;
	GLenum type;
}

@property (readonly, assign) GLuint shader;
@property (readonly, assign) GLenum type;

+ (NSString *)fileExtensionForShaderType:(GLenum)aShaderType;

- (id)initWithShaderType:(GLenum)aShaderType
		resourceInBundle:(NSString *)resourceInBundle;

@end
