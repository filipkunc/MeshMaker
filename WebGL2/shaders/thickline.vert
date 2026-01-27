#version 300 es
precision highp float;

// Vertex attributes
layout (location = 0) in vec3 aPosition;      // Current point
layout (location = 1) in vec3 aNextPosition;  // Next point in line/circle
layout (location = 2) in float aSide;         // -1.0 or +1.0 (which side of the line)

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uAxisTransform;
uniform vec2 uViewportSize;    // Viewport width and height in pixels
uniform float uLineWidth;      // Line width in pixels

// Clip plane support
uniform vec4 uClipPlane;
uniform bool uClipEnabled;

out float vClipDistance;

void main() {
    // Transform both points to clip space
    mat4 mvp = uProjection * uView * uModel * uAxisTransform;
    vec4 clipPos = mvp * vec4(aPosition, 1.0);
    vec4 clipNext = mvp * vec4(aNextPosition, 1.0);
    
    // Convert to NDC (normalized device coordinates)
    vec2 ndcPos = clipPos.xy / clipPos.w;
    vec2 ndcNext = clipNext.xy / clipNext.w;
    
    // Convert to screen space
    vec2 screenPos = ndcPos * uViewportSize * 0.5;
    vec2 screenNext = ndcNext * uViewportSize * 0.5;
    
    // Calculate line direction in screen space
    vec2 lineDir = screenNext - screenPos;
    float lineLength = length(lineDir);
    
    // Handle degenerate case (points very close together)
    vec2 perpendicular;
    if (lineLength < 0.001) {
        perpendicular = vec2(1.0, 0.0);
    } else {
        // Normalize and get perpendicular
        lineDir = lineDir / lineLength;
        perpendicular = vec2(-lineDir.y, lineDir.x);
    }
    
    // Offset in screen space by half the line width
    vec2 offset = perpendicular * (uLineWidth * 0.5) * aSide;
    
    // Convert offset back to NDC
    vec2 ndcOffset = offset / (uViewportSize * 0.5);
    
    // Apply offset in clip space (multiply by w to stay in clip space)
    vec4 finalPos = clipPos;
    finalPos.xy += ndcOffset * clipPos.w;
    
    gl_Position = finalPos;
    
    // Calculate clip distance for clipping support
    vec4 worldPos = uModel * uAxisTransform * vec4(aPosition, 1.0);
    if (uClipEnabled) {
        vClipDistance = dot(worldPos.xyz, uClipPlane.xyz) + uClipPlane.w;
    } else {
        vClipDistance = 1.0;
    }
}
