#version 300 es
precision highp float;

// UV Background fragment shader - samples the texture

in vec2 vTexCoord;

out vec4 fragColor;

uniform sampler2D uTexture;
uniform bool uHasTexture;
uniform float uAlpha;

void main() {
    if (uHasTexture) {
        vec4 texColor = texture(uTexture, vTexCoord);
        fragColor = vec4(texColor.rgb, texColor.a * uAlpha);
    } else {
        // Checkerboard pattern when no texture
        float checker = mod(floor(vTexCoord.x * 8.0) + floor(vTexCoord.y * 8.0), 2.0);
        float gray = mix(0.2, 0.25, checker);
        fragColor = vec4(gray, gray, gray, uAlpha);
    }
}
