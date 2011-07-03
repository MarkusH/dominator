varying vec4 shadowCoords;
uniform float shadowTexel;
uniform float yPixelOffset;

varying vec3 v;
varying vec3 lightvec;
varying vec3 normal;

uniform sampler2D Texture0;
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
	vec3 Reflected = normalize(reflect( -lightvec, normal)); 
	vec4 IAmbient = gl_LightSource[0].ambient * gl_FrontMaterial.ambient;
	vec4 IDiffuse = gl_LightSource[0].diffuse * max(dot(normal, lightvec), 0.0) * gl_FrontMaterial.diffuse;

	IDiffuse *= shadow;

	gl_FragColor = vec4((gl_FrontLightModelProduct.sceneColor * min(shadow + 0.2, 1.0) + IAmbient + IDiffuse) * texture2D(Texture0, vec2(gl_TexCoord[0])));
}
