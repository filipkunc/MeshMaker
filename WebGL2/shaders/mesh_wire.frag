#version 300 es
precision highp float;

in vec3 vNormal;
in vec3 vEyeCoords;
in vec3 vColor;
in vec2 vTexCoord;
in vec3 vBary;
in vec3 vEdgeMask;
in vec3 vEdgeState;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform bool uUseTexture;
uniform vec4 uBaseColor;
uniform float uMetallic;
uniform float uRoughness;
uniform vec3 uWireColor;
uniform vec3 uSelectionColor;
uniform vec3 uSeamColor;
uniform float uLineWidth;
uniform bool uShowSeams;

void main() {
    // === Solid shading (same as mesh.frag) ===
    vec3 l, n;
    if (!gl_FrontFacing) {
        l = vec3(0.0, 0.0, 1.0);
        n = normalize(-vNormal);
    } else {
        l = vec3(0.0, 0.0, -1.0);
        n = normalize(vNormal);
    }
    
    vec3 material;
    if (uUseTexture) {
        vec4 texColor = texture(uTexture, vTexCoord);
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
        float shininess = mix(128.0, 4.0, clamp(uRoughness, 0.0, 1.0));
        vec3 specularColor = mix(vec3(0.04), material, clamp(uMetallic, 0.0, 1.0));
        specular = specularColor * pow(max(dot(r, v), 0.0), shininess);
    }
    
    vec3 solidColor = min(ambient + diffuse + specular, vColor * 1.8);
    
    // === Wireframe overlay ===
    // Compute screen-space distance to each triangle edge using derivatives
    // For each barycentric coord b[i], distance to edge i = b[i] / fwidth(b[i])
    // Mask out quad diagonal edges using edgeMask
    vec3 d = vBary / fwidth(vBary);
    
    // Apply edge mask: set distance to large value for hidden edges (quad diagonals)
    d = mix(vec3(1e6), d, vEdgeMask);
    
    // Minimum distance to any visible edge
    float minDist = min(min(d.x, d.y), d.z);
    
    // Determine which edge is closest and its state
    float state = 0.0;
    if (d.x <= d.y && d.x <= d.z) {
        state = vEdgeState.x;
    } else if (d.y <= d.z) {
        state = vEdgeState.y;
    } else {
        state = vEdgeState.z;
    }
    
    // Pick wire color based on edge state: 0=normal, 1=selected, 2=seam, 3=selected+seam
    vec3 wireColor = uWireColor;
    if (state > 2.5) {
        wireColor = uShowSeams ? vec3(1.0, 0.0, 0.0) : uSelectionColor;  // selected+seam = red (or orange if seams hidden)
    } else if (state > 1.5) {
        if (uShowSeams) wireColor = uSeamColor;  // seam only - show green or keep default
    } else if (state > 0.5) {
        wireColor = uSelectionColor;  // selected only
    }
    
    // Antialiased edge factor: 1.0 on edge, 0.0 away from edge
    // Half line width for the distance threshold, +1 pixel for AA zone
    float halfWidth = uLineWidth * 0.5;
    float edgeFactor = 1.0 - smoothstep(halfWidth - 0.5, halfWidth + 0.5, minDist);
    
    // Blend wire color over solid color
    vec3 finalColor = mix(solidColor, wireColor, edgeFactor);
    FragColor = vec4(finalColor, uBaseColor.a);
}
