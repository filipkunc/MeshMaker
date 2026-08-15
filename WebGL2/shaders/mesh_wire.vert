#version 300 es
precision highp float;

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoord;
layout (location = 4) in vec3 aBary;
layout (location = 5) in vec3 aEdgeMask;
layout (location = 6) in vec3 aEdgeState;
layout (location = 7) in vec4 aTangent;

out vec3 vNormal;
out vec3 vEyeCoords;
out vec3 vColor;
out vec2 vTexCoord;
out vec3 vBary;
out vec3 vEdgeMask;
out vec3 vEdgeState;
out vec3 vTangent;
out vec3 vBitangent;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;

void main() {
    vec4 eyePos = uView * uModel * vec4(aPosition, 1.0);
    vEyeCoords = vec3(eyePos);
    vNormal = normalize(uNormalMatrix * aNormal);
    vTangent = normalize(uNormalMatrix * aTangent.xyz);
    vBitangent = normalize(cross(vNormal, vTangent) * aTangent.w);
    vColor = aColor;
    vTexCoord = aTexCoord;
    vBary = aBary;
    vEdgeMask = aEdgeMask;
    vEdgeState = aEdgeState;
    
    gl_Position = uProjection * eyePos;
}
