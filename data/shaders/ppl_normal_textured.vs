varying vec4 shadowCoords;
varying vec3 v;
varying vec3 lightvec;
varying vec3 normal;

void main()
{
	normal = normalize(gl_NormalMatrix * gl_Normal);
    vec4 pos = gl_ModelViewMatrix * gl_Vertex;
	v = pos.xyz;
	lightvec = normalize(gl_LightSource[0].position.xyz - v);
 
	gl_TexCoord[0] = gl_MultiTexCoord0;

	gl_Position = gl_ProjectionMatrix * pos;
	shadowCoords = gl_TextureMatrix[7] * pos;
}
