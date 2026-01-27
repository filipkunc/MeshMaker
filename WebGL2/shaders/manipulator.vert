#version 300 es
precision highp float;

layout (location = 0) in vec3 aPosition;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uAxisTransform;

// Clip plane support for rotation manipulator
uniform vec4 uClipPlane;
uniform bool uClipEnabled;

out float vClipDistance;

void main() {
    vec4 worldPos = uModel * uAxisTransform * vec4(aPosition, 1.0);
    gl_Position = uProjection * uView * worldPos;
    
    // Calculate clip distance (positive = keep, negative = clip)
    if (uClipEnabled) {
        vClipDistance = dot(worldPos.xyz, uClipPlane.xyz) + uClipPlane.w;
    } else {
        vClipDistance = 1.0; // Always visible when clipping disabled
    }
}
