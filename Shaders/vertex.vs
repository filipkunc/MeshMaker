// vertex.vs

varying vec3 normal;
varying vec3 eyeCoords;

void main()
{	
	normal = normalize(gl_NormalMatrix * gl_Normal);
    eyeCoords = vec3(gl_ModelViewMatrix * gl_Vertex);
    
	gl_FrontColor = gl_Color;
    gl_Position = ftransform();
}
