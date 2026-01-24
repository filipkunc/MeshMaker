#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;

out vec3 vNormal;
out vec3 vFragPos;
out vec3 vColor;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;

void main() {
    vFragPos = vec3(uModel * vec4(aPosition, 1.0));
    vNormal = uNormalMatrix * aNormal;
    vColor = aColor;
    
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
}
