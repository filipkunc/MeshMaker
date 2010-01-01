// twoSidedLighting.vs

void main()
{	
	// modified code from tutorial at http://www.lighthouse3d.com/opengl/glsl/index.php?ogldir1
	// ignores material info, gets color via GL_COLOR_ARRAY in Mesh.mm
	// added two sided lighting
	
	vec3 normal;
	float NdotL;
	float BackNdotL;
	
	normal = normalize(gl_NormalMatrix * gl_Normal);
	NdotL = max(dot(normal, vec3(0.0, 0.0, 1.0)), 0.0);
	BackNdotL = max(dot(normal, vec3(0.0, 0.0, -1.0)), 0.0);
	
	gl_FrontColor = vec4(0.5, 0.5, 0.5, 1.0) * NdotL + vec4(0.2, 0.2, 0.2, 1.0);
	
	gl_BackColor = gl_Color * BackNdotL + vec4(0.25, 0.25, 0.25, 1.0);
	
	gl_Position = ftransform();
}
