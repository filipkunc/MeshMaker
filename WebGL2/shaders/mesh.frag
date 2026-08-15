#version 300 es
precision highp float;

// Ported from original MeshMaker fragment.fs

in vec3 vNormal;
in vec3 vEyeCoords;
in vec3 vColor;
in vec2 vTexCoord;
in vec3 vTangent;
in vec3 vBitangent;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform bool uUseTexture;
uniform vec4 uBaseColor;
uniform float uMetallic;
uniform float uRoughness;
uniform sampler2D uNormalTexture;
uniform bool uUseNormalTexture;
uniform vec3 uEmissiveColor;
uniform float uClearcoat;
uniform float uClearcoatRoughness;
uniform float uIor;

void main() {
    vec3 l, n;
    
    // Match original: different light direction for front/back faces
    // But use vColor for both to show selection highlighting
    if (!gl_FrontFacing) {
        l = vec3(0.0, 0.0, 1.0);
        n = normalize(-vNormal);
    } else {
        l = vec3(0.0, 0.0, -1.0);
        n = normalize(vNormal);
    }
    if (uUseNormalTexture) {
        vec3 mapped = texture(uNormalTexture, vTexCoord).xyz * 2.0 - 1.0;
        n = normalize(mat3(normalize(vTangent), normalize(vBitangent), n) * mapped);
    }
    
    // Get base material color - either from texture or vertex color
    vec3 material;
    if (uUseTexture) {
        vec4 texColor = texture(uTexture, vTexCoord);
        // Multiply texture color with vertex color to allow selection tinting
        material = texColor.rgb * uBaseColor.rgb * vColor;
    } else {
        material = uBaseColor.rgb * vColor;
    }
    
    vec3 s = normalize(l - vEyeCoords);
    vec3 v = normalize(-vEyeCoords);
    vec3 r = reflect(-s, n);
    
    float sDotN = max(dot(s, n), 0.0);
    
    vec3 ambient = vec3(0.3, 0.3, 0.3);
    vec3 diffuse = material * sDotN;
    vec3 specular = vec3(0.0);
    
    if (sDotN > 0.0) {
        // Match original: specular = material * pow(...)
        float shininess = mix(128.0, 4.0, clamp(uRoughness, 0.0, 1.0));
        float dielectricF0 = pow((max(uIor, 1.0) - 1.0) / (max(uIor, 1.0) + 1.0), 2.0);
        vec3 specularColor = mix(vec3(dielectricF0), material, clamp(uMetallic, 0.0, 1.0));
        specular = specularColor * pow(max(dot(r, v), 0.0), shininess);
        float coatShininess = mix(256.0, 8.0, clamp(uClearcoatRoughness, 0.0, 1.0));
        specular += vec3(0.04 * clamp(uClearcoat, 0.0, 1.0)) *
            pow(max(dot(r, v), 0.0), coatShininess);
    }
    
    vec3 finalColor = min(ambient + diffuse + specular + uEmissiveColor, vColor * 1.8 + uEmissiveColor);
    FragColor = vec4(finalColor, uBaseColor.a);
}
