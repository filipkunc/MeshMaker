#version 300 es
precision highp float;

// Fragment shader for thick lines with per-vertex color and antialiasing

in vec3 vColor;
in float vSide;

uniform bool uAntialias;
uniform float uLineWidth;

out vec4 FragColor;

void main() {
    if (uAntialias) {
        // The quad is expanded by 1px beyond uLineWidth for AA.
        // vSide goes from -1 to +1 across the expanded quad.
        // The core line edge in normalized space:
        float totalWidth = uLineWidth + 1.0;
        float coreEdge = uLineWidth / totalWidth;
        float dist = abs(vSide);
        float alpha = 1.0 - smoothstep(coreEdge, 1.0, dist);
        FragColor = vec4(vColor, alpha);
    } else {
        // Solid color (for selection picking)
        FragColor = vec4(vColor, 1.0);
    }
}
