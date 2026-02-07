#version 300 es
precision highp float;

// UV Editor vertex shader - uses UV coordinates as XY position
// Renders mesh faces in UV space for editing

layout(location = 0) in vec3 aPosition;  // Original 3D position (unused but needed for buffer layout)
layout(location = 1) in vec3 aNormal;    // Normal (unused)
layout(location = 2) in vec3 aColor;     // Selection/face color
layout(location = 3) in vec2 aTexCoord;  // UV coordinates - used as XY position

out vec3 vColor;
out vec2 vTexCoord;

uniform mat4 uProjection;
uniform vec2 uOffset;        // Pan offset in UV space
uniform float uZoom;         // Zoom factor
uniform vec2 uAspectAdjust;  // Aspect ratio correction

void main() {
    // Use UV coordinates as XY position in clip space
    // UV space is 0-1, we map to -1 to 1 clip space with zoom and offset
    vec2 pos = (aTexCoord + uOffset) * uZoom;
    
    // Map from 0-1 to -1..1 clip space (centered)
    pos = pos * 2.0 - vec2(1.0);
    
    // Apply aspect ratio correction to keep UV space square
    pos *= uAspectAdjust;
    
    gl_Position = vec4(pos, 0.0, 1.0);
    vColor = aColor;
    vTexCoord = aTexCoord;
}
