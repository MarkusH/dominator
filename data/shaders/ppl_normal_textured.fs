varying vec3 v;
varying vec3 lightvec;
varying vec3 normal;

uniform sampler2D Texture0;
uniform sampler2D Texture1;

void main()
{
	vec3 Eye = normalize(-v);
	vec2 uv = vec2(gl_TexCoord[0]);

	vec3 q0 = dFdx(Eye.xyz);
	vec3 q1 = dFdy(Eye.xyz);
	vec2 st0 = dFdx(uv.st);
	vec2 st1 = dFdy(uv.st);
 
	vec3 S = normalize( q0 * st1.t - q1 * st0.t);
	vec3 T = normalize(-q0 * st1.s + q1 * st0.s);
 
	mat3 M = mat3(-T, -S, normal);
	vec3 norm = (texture2D(Texture1, uv).xyz - 0.5) * 2.0;
	norm = normalize(M * norm);

	vec3 Reflected    = normalize(reflect(-lightvec, norm));

	vec4 IAmbient  = gl_LightSource[0].ambient * gl_FrontMaterial.ambient;
	vec4 IDiffuse  = gl_LightSource[0].diffuse * max(dot(norm, lightvec), 0.0) * gl_FrontMaterial.diffuse;
	vec4 ISpecular = gl_LightSource[0].specular * pow(max(dot(Reflected, Eye), 0.0), gl_FrontMaterial.shininess) * gl_FrontMaterial.specular;
 
	gl_FragColor = vec4((gl_FrontLightModelProduct.sceneColor + IAmbient + IDiffuse) * texture2D(Texture0, uv) + ISpecular);
}
