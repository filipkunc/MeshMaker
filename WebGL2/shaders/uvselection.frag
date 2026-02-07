#version 300 es
precision highp float;

// UV Selection fragment shader - outputs color index for picking

out vec4 fragColor;

uniform uint uColorIndex;

void main() {
    // Encode index as RGB color
    uint r = uColorIndex & 255u;
    uint g = (uColorIndex >> 8u) & 255u;
    uint b = (uColorIndex >> 16u) & 255u;
    
    fragColor = vec4(float(r) / 255.0, float(g) / 255.0, float(b) / 255.0, 1.0);
}
