#version 300 es
precision highp float;

// UV Background vertex shader - draws a textured quad for the UV space background

layout(location = 0) in vec2 aPosition;  // Quad vertices (0-1 range)

out vec2 vTexCoord;

uniform vec2 uOffset;        // Pan offset in UV space
uniform float uZoom;         // Zoom factor
uniform vec2 uAspectAdjust;  // Aspect ratio correction

void main() {
    // Pass through texture coordinates (0-1)
    vTexCoord = aPosition;
    
    // Transform position same as UV grid
    vec2 pos = (aPosition + uOffset) * uZoom;
    
    // Map from 0-1 to -1..1 clip space (centered)
    pos = pos * 2.0 - vec2(1.0);
    
    // Apply aspect ratio correction
    pos *= uAspectAdjust;
    
    gl_Position = vec4(pos, 0.0, 1.0);
}
