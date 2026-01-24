#version 300 es
precision highp float;

layout (location = 0) in vec3 aPosition;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uAxisTransform;

void main() {
    gl_Position = uProjection * uView * uModel * uAxisTransform * vec4(aPosition, 1.0);
}
