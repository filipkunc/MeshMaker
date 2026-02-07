#version 300 es
precision highp float;

// UV Grid vertex shader - draws the 0-1 UV space grid

layout(location = 0) in vec2 aPosition;

uniform vec2 uOffset;
uniform float uZoom;
uniform vec2 uAspectAdjust;

void main() {
    vec2 pos = (aPosition + uOffset) * uZoom;
    pos = pos * 2.0 - vec2(1.0);
    pos *= uAspectAdjust;
    gl_Position = vec4(pos, 0.0, 1.0);
}
