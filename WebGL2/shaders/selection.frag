#version 300 es
precision highp float;
precision highp int;

// Selection shader: outputs color as ID for color-buffer picking
// Each selectable element is drawn with a unique color

uniform uint uColorIndex;
uniform vec4 uColorOverride;
uniform bool uUseColorOverride;

out vec4 FragColor;

void main() {
    if (uUseColorOverride) {
        FragColor = uColorOverride;
    } else {
        // Encode index as RGB (matching original: glColor4ubv((GLubyte*)&colorIndex))
        // Index 0 = background (no selection)
        // Index 1+ = selectable elements
        // We use RGB for the index and set alpha to 1.0 to avoid blending issues
        uint r = uColorIndex & 0xFFu;
        uint g = (uColorIndex >> 8) & 0xFFu;
        uint b = (uColorIndex >> 16) & 0xFFu;
        
        FragColor = vec4(float(r) / 255.0, float(g) / 255.0, float(b) / 255.0, 1.0);
    }
}
