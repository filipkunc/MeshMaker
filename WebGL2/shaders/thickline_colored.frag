#version 300 es
precision highp float;

// Fragment shader for thick lines with per-vertex color and antialiasing

in vec3 vColor;
in float vSide;

uniform bool uAntialias;

out vec4 FragColor;

void main() {
    if (uAntialias) {
        // Smooth antialiased edges
        float dist = abs(vSide);
        float alpha = 1.0 - smoothstep(0.5, 1.0, dist);
        FragColor = vec4(vColor, alpha);
    } else {
        // Solid color (for selection picking)
        FragColor = vec4(vColor, 1.0);
    }
}
