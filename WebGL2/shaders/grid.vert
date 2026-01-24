#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;

out vec3 vColor;

uniform mat4 uView;
uniform mat4 uProjection;

void main() {
    vColor = aColor;
    gl_Position = uProjection * uView * vec4(aPosition, 1.0);
}
