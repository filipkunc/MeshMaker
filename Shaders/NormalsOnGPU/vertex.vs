void main()
{	
	vec3 normal, lightDir;
	vec4 colorAdd = vec4(0.25, 0.25, 0.25, 0.0);
	float NdotL;
	
	normal = normalize(gl_NormalMatrix * gl_Normal);
	lightDir = vec3(0.0, 0.0, -1.0);
	NdotL = max(dot(normal, lightDir), 0.0);
	
	gl_FrontColor = gl_Color * NdotL + colorAdd;
	gl_FrontColor.a = 1.0;
	
	gl_Position = ftransform();
}
