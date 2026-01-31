#version 300 es
precision highp float;

// Ported from original MeshMaker fragment.fs

in vec3 vNormal;
in vec3 vEyeCoords;
in vec3 vColor;
in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform bool uUseTexture;

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
    
    // Get base material color - either from texture or vertex color
    vec3 material;
    if (uUseTexture) {
        vec4 texColor = texture(uTexture, vTexCoord);
        // Multiply texture color with vertex color to allow selection tinting
        material = texColor.rgb * vColor;
    } else {
        material = vColor;
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
        specular = material * pow(max(dot(r, v), 0.0), 40.0);
    }
    
    vec3 finalColor = min(ambient + diffuse + specular, vColor * 1.8);
    FragColor = vec4(finalColor, 1.0);
}
