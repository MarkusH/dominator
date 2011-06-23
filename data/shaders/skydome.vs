varying float intensity_sq, intensity;

uniform float time;
uniform float invRadius;
uniform float invHeight;

void main()
{

   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

   gl_TexCoord[0] = vec4(gl_Vertex.x * invRadius       + time, gl_Vertex.z * invRadius,
       					 gl_Vertex.x * invRadius * 2.0 + time,       gl_Vertex.z * invRadius * 2.0);


   //gl_TexCoord[1] = vec4(gl_Vertex.x * invRadius * 2.0 + time, gl_Vertex.z * invRadius * 2.0, 0.0, 0.0);

   intensity    = (gl_Vertex.y/16.0-1.0) * invHeight;
   intensity    = exp(clamp(intensity, 0.0, 1.0)) / 1.8;
   intensity_sq = intensity * intensity;
}

