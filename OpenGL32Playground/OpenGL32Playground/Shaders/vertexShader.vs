//#version 130

//in vec4 inPosition;

uniform mat4 inMvp;

void main(void)
{
    gl_Position = inMvp * gl_Vertex;
}