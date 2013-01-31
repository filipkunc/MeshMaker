// textured_frag.fs

uniform sampler2D texture;
varying vec3 normal;
varying vec3 eyeCoords;

void main()
{
    vec3 l, n;
    vec4 baseColor;
    
    if (!gl_FrontFacing)
    {
        l = vec3(0.0, 0.0, 1.0);
        n = normalize(-normal);
        baseColor = gl_Color;
    }
    else
    {
        l = vec3(0.0, 0.0, -1.0);
        n = normalize(normal);
        baseColor = vec4(0.5, 0.5, 0.5, 0.0);
    }
    
    vec4 material = baseColor;
    
    vec4 textureColor = texture2D(texture, gl_TexCoord[0].st);
    material.r *= textureColor.r;
    material.g *= textureColor.g;
    material.b *= textureColor.b;
    material.a *= textureColor.a;
    
    vec3 s = normalize(l - eyeCoords);
    vec3 v = normalize(-eyeCoords);
    vec3 r = reflect(-s, n);
    
    float sDotN = max(dot(s, n), 0.0);
    
    vec4 ambient = vec4(0.3, 0.3, 0.3, 0.0);
    vec4 diffuse = material * sDotN;
    vec4 specular = vec4(0.0, 0.0, 0.0, 0.0);
    
    if (sDotN > 0.0)
        specular = vec4(0.3, 0.3, 0.3, 0.0) * pow(max(dot(r, v), 0.0), 20.0);
    
    vec4 finalColor = min(ambient + diffuse + specular, baseColor * 1.8);
    gl_FragColor = finalColor;
}