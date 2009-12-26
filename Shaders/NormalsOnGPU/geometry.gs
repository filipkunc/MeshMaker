#version 120 
#extension GL_EXT_geometry_shader4 : enable

vec3 NormalFromTriangleVertices(vec3 triangleVertices[3])
{
	// now is same as RedBook (OpenGL Programming Guide)
	vec3 u = triangleVertices[0] - triangleVertices[1];
	vec3 v = triangleVertices[1] - triangleVertices[2];
	return cross(v, u);
}

void main()
{
	// modified code from tutorial at http://www.lighthouse3d.com/opengl/glsl/index.php?ogldir1
	// no change of position
	// computes normal from input triangle and front color for that triangle
	
	vec3 triangleVertices[3];
	vec3 computedNormal;

	vec3 normal, lightDir;
	vec4 diffuse;
	float NdotL;

	vec4 finalColor;

	for(int i = 0; i < gl_VerticesIn; i += 3)
	{
		for (int j = 0; j < 3; j++)
		{
			triangleVertices[j] = gl_PositionIn[i + j].xyz;
        }
		computedNormal = NormalFromTriangleVertices(triangleVertices);
		normal = normalize(gl_NormalMatrix * computedNormal);
	
		// hardcoded light direction	
		vec4 light = gl_ModelViewMatrix * vec4(0.0, 0.0, 1.0, 0.0);
		lightDir = normalize(light.xyz);
		
		NdotL = max(dot(normal, lightDir), 0.0);
		
		// hardcoded
		diffuse = vec4(0.5, 0.5, 0.9, 1.0);

		finalColor = NdotL * diffuse; 
		finalColor.a = 1.0; // final color ignores everything, except lighting
	
		for (int j = 0; j < 3; j++)
		{	
			gl_FrontColor = finalColor;
			gl_Position = gl_PositionIn[i + j];
			EmitVertex();
		}
	}
	EndPrimitive();
}
