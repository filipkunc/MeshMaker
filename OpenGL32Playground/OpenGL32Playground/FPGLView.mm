//
//  FPGLView.m
//  OpenGL32Playground
//
//  Created by Filip Kunc on 7/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "FPGLView.h"

static inline const char * GetGLErrorString(GLenum error)
{
	const char *str;
	switch( error )
	{
		case GL_NO_ERROR:
			str = "GL_NO_ERROR";
			break;
		case GL_INVALID_ENUM:
			str = "GL_INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			str = "GL_INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			str = "GL_INVALID_OPERATION";
			break;		
#if defined __gl_h_ || defined __gl3_h_
		case GL_OUT_OF_MEMORY:
			str = "GL_OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			str = "GL_INVALID_FRAMEBUFFER_OPERATION";
			break;
#endif
#if defined __gl_h_
		case GL_STACK_OVERFLOW:
			str = "GL_STACK_OVERFLOW";
			break;
		case GL_STACK_UNDERFLOW:
			str = "GL_STACK_UNDERFLOW";
			break;
		case GL_TABLE_TOO_LARGE:
			str = "GL_TABLE_TOO_LARGE";
			break;
#endif
		default:
			str = "(ERROR: Unknown Error Enum)";
			break;
	}
	return str;
}

#define GetGLError()									\
{														\
	GLenum err = glGetError();							\
	while (err != GL_NO_ERROR) {						\
		NSLog(@"GLError %s set in File:%s Line:%d\n",	\
				GetGLErrorString(err),					\
				__FILE__,								\
				__LINE__);								\
		err = glGetError();								\
	}													\
}

struct Vector3D 
{
    float x, y, z;
};

struct Triangle
{
    Vector3D vertices[3];
};

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

@implementation FPGLView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (void)awakeFromNib
{
    NSOpenGLPixelFormatAttribute attrs[] =
	{
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFADepthSize, 24,
		// Must specify the 3.2 Core Profile to use OpenGL 3.2
		NSOpenGLPFAOpenGLProfile,
		NSOpenGLProfileVersion3_2Core,
		0
	};
	
	NSOpenGLPixelFormat *pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
	
	if (!pf)
	{
		NSLog(@"No OpenGL pixel format");
	}
    
    NSOpenGLContext* context = [[NSOpenGLContext alloc] initWithFormat:pf shareContext:nil];
    
    [self setPixelFormat:pf];
    
    [self setOpenGLContext:context];
}

- (GLuint)loadShaderWithName:(NSString *)fileName type:(GLenum)shaderType
{
    NSBundle *bundle = [NSBundle mainBundle];

    NSString *path = [bundle pathForResource:[fileName stringByDeletingPathExtension] ofType:[fileName pathExtension]];
    
    NSString *contents;
    contents = [NSString stringWithContentsOfFile:path
                                         encoding:NSUTF8StringEncoding
                                            error:NULL];
    
    const GLchar *shaderSource = [contents cStringUsingEncoding:NSASCIIStringEncoding];
    
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);
    
    GLint logLength, status;
    
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0) 
	{
		GLchar *log = (GLchar*)malloc(logLength);
		glGetShaderInfoLog(shader, logLength, &logLength, log);
		NSLog(@"%@ compile log:\n%s\n", fileName, log);
		free(log);
	}
	
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == 0)
	{
		NSLog(@"Failed to compile %@ shader:\n%s\n", fileName, shaderSource);
		return 0;
	}
    
    return shader;
}

- (void)createShaderProgram
{
    _shaderProgram = glCreateProgram();
    glBindAttribLocation(_shaderProgram, 0, "inPosition");
    
    _vertexShader = [self loadShaderWithName:@"vertexShader.vs" type:GL_VERTEX_SHADER];
    glAttachShader(_shaderProgram, _vertexShader);
    
    _fragmentShader = [self loadShaderWithName:@"fragmentShader.fs" type:GL_FRAGMENT_SHADER];
    glAttachShader(_shaderProgram, _fragmentShader);
    
    glLinkProgram(_shaderProgram);
    
    GLint logLength, status;
    
    glGetProgramiv(_shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0)
	{
		GLchar *log = (GLchar*)malloc(logLength);
		glGetProgramInfoLog(_shaderProgram, logLength, &logLength, log);
		NSLog(@"Program link log:\n%s\n", log);
		free(log);
	}
	
	glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &status);
	if (status == 0)
	{
		NSLog(@"Failed to link program");
		return;
	}
	
	glValidateProgram(_shaderProgram);
	glGetProgramiv(_shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0)
	{
		GLchar *log = (GLchar*)malloc(logLength);
		glGetProgramInfoLog(_shaderProgram, logLength, &logLength, log);
		NSLog(@"Program validate log:\n%s\n", log);
		free(log);
	}
	
	glGetProgramiv(_shaderProgram, GL_VALIDATE_STATUS, &status);
	if (status == 0)
	{
		NSLog(@"Failed to validate program");
		return;
	}
    
    glUseProgram(_shaderProgram);
    
    GetGLError();
}

- (void)prepareOpenGL
{
    [super prepareOpenGL];
    
    glGenVertexArrays(1, &_vertexArray);
    glBindVertexArray(_vertexArray);
    
    glGenBuffers(1, &_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
    
    Triangle t;
    t.vertices[0] = (Vector3D) { -100.0f, -50.0f, -10.0f };
    t.vertices[1] = (Vector3D) { 100.0f, -50.0f, 100.0f };
    t.vertices[2] = (Vector3D) { 50.0f, 50.0f, 10.0f };
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(t) * 1, &t, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    
    GetGLError();
    
    [self createShaderProgram];
    
    _mvpLocation = glGetUniformLocation(_shaderProgram, "inMvp");
}

- (void)drawRect:(NSRect)dirtyRect
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    NSRect rect = [self bounds];
    glViewport(0, 0, (int)rect.size.width, (int)rect.size.height);
    const float clearColor = 0.6f;
	glClearColor(clearColor, clearColor, clearColor, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    
    GLfloat mvp[16] =
    {
        1,
        0,
        0,
        0,
        0,
        -4.37114e-08,
        1.001,
        1,
        0,
        1,
         4.37551e-08,
         4.37114e-08,
         0,
         150,
         440.445,
         450,
    };
    
    glUseProgram(_shaderProgram);
    
    glUniformMatrix4fv(_mvpLocation, 1, GL_FALSE, mvp);
    
    glBindVertexArray(_vertexArray);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    GetGLError();
    
    [[self openGLContext] flushBuffer];
}

@end
