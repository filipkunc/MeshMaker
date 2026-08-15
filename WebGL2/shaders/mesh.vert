#version 300 es
precision highp float;

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoord;
layout (location = 4) in vec4 aTangent;

out vec3 vNormal;
out vec3 vEyeCoords;
out vec3 vColor;
out vec2 vTexCoord;
out vec3 vTangent;
out vec3 vBitangent;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;

void main() {
    // Transform to eye/view space (matching original vertex.vs)
    vec4 eyePos = uView * uModel * vec4(aPosition, 1.0);
    vEyeCoords = vec3(eyePos);
    vNormal = normalize(uNormalMatrix * aNormal);
    vTangent = normalize(uNormalMatrix * aTangent.xyz);
    vBitangent = normalize(cross(vNormal, vTangent) * aTangent.w);
    vColor = aColor;
    vTexCoord = aTexCoord;
    
    gl_Position = uProjection * eyePos;
}
