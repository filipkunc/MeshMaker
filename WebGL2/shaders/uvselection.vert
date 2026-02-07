#version 300 es
precision highp float;

// UV Selection vertex shader - for color-picking in UV space

layout(location = 0) in vec2 aPosition;  // UV coordinates as position

uniform vec2 uOffset;        // Pan offset in UV space
uniform float uZoom;         // Zoom factor
uniform vec2 uAspectAdjust;  // Aspect ratio correction

void main() {
    // Transform UV position same as regular UV shader
    vec2 pos = (aPosition + uOffset) * uZoom;
    
    // Map from 0-1 to -1..1 clip space (centered)
    pos = pos * 2.0 - vec2(1.0);
    
    // Apply aspect ratio correction
    pos *= uAspectAdjust;
    
    gl_Position = vec4(pos, 0.0, 1.0);
}
