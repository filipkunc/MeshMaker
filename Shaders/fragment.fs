// fragment.fs

varying vec3 normal;

void main()
{
    vec3 n = normalize(normal);
    float NdotL = max(dot(n, vec3(0.0, 0.0, -1.0)), 0.0);
    gl_FragColor = gl_Color * NdotL + vec4(0.3, 0.3, 0.3, 1.0);
    gl_FragColor.a = 0.5;
}