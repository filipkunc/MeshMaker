#version 300 es
precision highp float;

// UV Editor fragment shader

in vec3 vColor;
in vec2 vTexCoord;

out vec4 fragColor;

uniform sampler2D uTexture;
uniform bool uHasTexture;
uniform float uAlpha;

void main() {
    vec3 color = vColor;
    
    // Blend with texture if available
    if (uHasTexture) {
        vec4 texColor = texture(uTexture, vTexCoord);
        color = mix(color, texColor.rgb, texColor.a * 0.5);
    }
    
    fragColor = vec4(color, uAlpha);
}
