#version 330 core

in vec3 vNormal;
in vec3 vFragPos;
in vec3 vColor;

out vec4 FragColor;

uniform vec3 uLightDir;
uniform vec3 uLightColor;
uniform vec3 uAmbientColor;
uniform vec3 uViewPos;

void main() {
    // Normalize interpolated normal
    vec3 normal = normalize(vNormal);
    
    // Flip normal for back faces (two-sided lighting)
    if (!gl_FrontFacing) {
        normal = -normal;
    }
    
    // Ambient
    vec3 ambient = uAmbientColor * vColor;
    
    // Diffuse
    vec3 lightDir = normalize(-uLightDir);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor * vColor;
    
    // Specular (Blinn-Phong)
    vec3 viewDir = normalize(uViewPos - vFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * uLightColor * 0.3;
    
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
