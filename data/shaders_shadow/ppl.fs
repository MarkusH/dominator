varying vec3 v;
varying vec3 lightvec;
varying vec3 normal;
varying vec3 position;

//uniform sampler2D Texture0;

void main()
{
	vec3 Eye          = normalize(-v);
	vec3 Reflected    = normalize(reflect(-lightvec, normal)); 
	float dist        = distance(gl_LightSource[0].position.xyz, position);
	float attenFactor	= 1.0 / (gl_LightSource[0].constantAttenuation + gl_LightSource[0].linearAttenuation * dist + gl_LightSource[0].quadraticAttenuation * dist * dist);
  
	//attenFactor = 1.0;

	vec4 IAmbient  = gl_LightSource[0].ambient * gl_FrontMaterial.ambient * attenFactor;
	vec4 IDiffuse  = gl_LightSource[0].diffuse * max(dot(normal, lightvec), 0.0) * gl_FrontMaterial.diffuse * attenFactor;
	vec4 ISpecular = gl_LightSource[0].specular * pow(max(dot(Reflected, Eye), 0.0), gl_FrontMaterial.shininess) * gl_FrontMaterial.specular * attenFactor;
 
	gl_FragColor = vec4(gl_FrontLightModelProduct.sceneColor + IAmbient + IDiffuse + ISpecular);
}
