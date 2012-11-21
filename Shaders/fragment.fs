// fragment.fs

varying vec3 normal;
varying vec3 eyeCoords;

void main()
{
    vec3 l, n, material;
    
    if (!gl_FrontFacing)
    {
        l = vec3(0.0, 0.0, 1.0);
        n = normalize(-normal);
        material = vec3(gl_Color);
    }
    else
    {
        l = vec3(0.0, 0.0, -1.0);
        n = normalize(normal);
        material = vec3(0.5, 0.5, 0.5);
    }
    
    vec3 s = normalize(l - eyeCoords);
    vec3 v = normalize(-eyeCoords);
    vec3 r = reflect(-s, n);
    
    float sDotN = max(dot(s, n), 0.0);
    
    vec3 ambient = vec3(0.3, 0.3, 0.3);
    vec3 diffuse = material * sDotN;
    vec3 specular = vec3(0.0, 0.0, 0.0);
    
    if (sDotN > 0.0)
        specular = material * pow(max(dot(r, v), 0.0), 20.0);
    
    vec3 finalColor = ambient + diffuse + specular;
    gl_FragColor = vec4(finalColor, 0.5);
}