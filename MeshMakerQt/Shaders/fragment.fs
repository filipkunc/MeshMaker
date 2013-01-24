// fragment.fs

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
    
    vec3 s = normalize(l - eyeCoords);
    vec3 v = normalize(-eyeCoords);
    vec3 r = reflect(-s, n);
    
    float sDotN = max(dot(s, n), 0.0);
    
    vec4 ambient = vec4(0.3, 0.3, 0.3, 0.0);
    vec4 diffuse = material * sDotN;
    vec4 specular = vec4(0.0, 0.0, 0.0, 0.0);
    
    if (sDotN > 0.0)
        specular = material * pow(max(dot(r, v), 0.0), 40.0);
    
    vec4 finalColor = min(ambient + diffuse + specular, baseColor * 1.8);
    gl_FragColor = finalColor;
}