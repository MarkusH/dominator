varying float intensity_sq, intensity;

uniform float time;
uniform float invRadius;
uniform float invHeight;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	float x = gl_Vertex.x * invRadius;
	float z = gl_Vertex.z * invRadius;
	gl_TexCoord[0] = vec4(x       + time, z,
						  x * 2.0 + time, z * 2.0);

   intensity    = (gl_Vertex.y/16.0-1.0) * invHeight;
   intensity    = exp(clamp(intensity, 0.8, 1.0)) / 1.8; // clamp 0.0f->1.0f
   //intensity_sq = intensity * intensity;
}

