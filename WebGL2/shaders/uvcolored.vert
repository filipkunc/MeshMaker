#version 300 es
precision highp float;

// UV Editor colored vertex shader - for drawing vertices/edges in UV space

layout(location = 0) in vec2 aUV;      // UV coordinate as position
layout(location = 1) in vec3 aColor;   // Vertex color

out vec3 vColor;

uniform vec2 uOffset;
uniform float uZoom;
uniform float uPointSize;
uniform vec2 uAspectAdjust;

void main() {
    vec2 pos = (aUV + uOffset) * uZoom;
    pos = pos * 2.0 - vec2(1.0);
    pos *= uAspectAdjust;
    gl_Position = vec4(pos, 0.0, 1.0);
    gl_PointSize = uPointSize;
    vColor = aColor;
}
