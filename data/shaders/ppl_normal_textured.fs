varying vec4 shadowCoords;
uniform float shadowTexel;
uniform float yPixelOffset;

varying vec3 v;
varying vec3 lightvec;
varying vec3 normal;

uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform sampler2DShadow ShadowMap;

float lookup(vec2 offset)
{
    return shadow2DProj(ShadowMap, shadowCoords + vec4(
			    offset.x * shadowTexel * shadowCoords.w, 
			    offset.y * shadowTexel * shadowCoords.w,
			    0.0005 - 0.005,
			    0.0)).w;
}
void main()
{

	float shadow = 0.0;
	
	//if (shadowCoords.w > 0.0)
	{
		// single lookup with texture filtering
		//shadow = lookup(vec2(0.0,0.0));
		
		// 4x4 lookup with pcf
		float x, y;
		for (y = -1.5; y <= 1.5 ; y += 1.0)
			for (x = -1.5; x <= 1.5; x += 1.0)
				shadow += lookup(vec2(x, y));
		
		shadow *= 0.0625;
	}

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
    IDiffuse *= shadow;

	gl_FragColor = vec4((gl_FrontLightModelProduct.sceneColor * min(shadow + 0.2, 1.0) + IAmbient + IDiffuse) * texture2D(Texture0, uv) + ISpecular);
}
