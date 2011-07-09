varying vec3 v;
varying vec3 lightvec;
varying vec3 normal;

void main()
{
	normal = normalize(gl_NormalMatrix * gl_Normal);
	v = vec3(gl_ModelViewMatrix * gl_Vertex);
	lightvec = normalize(gl_LightSource[0].position.xyz - v);
 
	gl_TexCoord[0] = gl_MultiTexCoord0;

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
