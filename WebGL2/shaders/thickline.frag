#version 300 es
precision highp float;

uniform vec4 uColor;

in float vClipDistance;

out vec4 FragColor;

void main() {
    // Discard fragments on the wrong side of the clip plane
    if (vClipDistance < 0.0) {
        discard;
    }
    FragColor = uColor;
}
