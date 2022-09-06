#version 440 core


uniform vec4 		objectColour;
uniform sampler2D 	mainTex;
uniform sampler2DShadow shadowTex;

uniform sampler2D wallTex ;

uniform vec3	lightPos;
uniform float	lightRadius;
uniform vec4	lightColour;

uniform vec3	cameraPos;

uniform bool hasTexture;
uniform bool isPaintable;

uniform sampler2D paintMap;
uniform vec4 paintColor;

in Vertex
{
	vec4 colour;
	vec2 texCoord;
	vec4 shadowProj;
	vec3 normal;
	vec3 worldPos;
} IN;

out vec4 fragColor;

void main(void)
{
	float scale = 0.1;
	vec3 blending = abs( IN.normal );
	blending = normalize(max(blending, 0.00001)); // Force weights to sum to 1.0
	float b = (blending.x + blending.y + blending.z);
	blending /= vec3(b, b, b);
	vec4 wallTexTriplanar = (texture ( wallTex , IN . worldPos.yz * scale ) * blending.x) + (texture ( wallTex , IN . worldPos.xz * scale ) * blending.y) + (texture ( wallTex , IN . worldPos.xy * scale ) * blending.z);



	float shadow = 1.0; // New !
	
	if( IN . shadowProj . w > 0.0) { // New !
		shadow = textureProj ( shadowTex , IN . shadowProj ) * 0.5f;
	}

	vec3  incident = normalize ( lightPos - IN.worldPos );
	float lambert  = max (0.0 , dot ( incident , IN.normal )) * 0.9; 
	
	vec3 viewDir = normalize ( cameraPos - IN . worldPos );
	vec3 halfDir = normalize ( incident + viewDir );

	float rFactor = max (0.0 , dot ( halfDir , IN.normal ));
	float sFactor = pow ( rFactor , 80.0 );
	
	vec4 albedo = IN.colour;
	
	if(hasTexture) {
	 albedo *= texture(mainTex, IN.texCoord);
	}
	
	albedo = wallTexTriplanar;
	
	albedo.rgb = pow(albedo.rgb, vec3(2.2));
	
	fragColor.rgb = albedo.rgb * 0.05f; //ambient
	
	fragColor.rgb += albedo.rgb * lightColour.rgb * lambert * shadow; //diffuse light
	
	fragColor.rgb += lightColour.rgb * sFactor * shadow; //specular light
	
	fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / 2.2f));
	
	fragColor.a = albedo.a;

	if(isPaintable)
	{
		float paintAlpha = 0.0;
		/*float texelSize = 1.0 / textureSize(paintMap, 0).x;
		for (int x = -1; x < 2; x++)
			for (int y = -1; y < 2; y++)
				paintAlpha += 1 - texture2D(paintMap, vec2(IN.texCoord.x + texelSize * x, IN.texCoord.y + texelSize * y)).r;

		paintAlpha /= 9;*/
		
		//paintAlpha = 1.0 - texture2D(paintMap, IN.texCoord).r;
		
		//paintAlpha = texture(paintMap,vec2(0.5f,0.8f)).r;
		//paintAlpha = texture(paintMap, IN.texCoord).r;
		
		
		//paintAlpha = texture2D(paintMap, vec2(IN.texCoord.x, IN.texCoord.y)).r;
		fragColor = mix(fragColor, paintColor, paintAlpha);
		
		
		//fragColor = vec4(fragColor.rgb, 1.0) * (1.0 - paintAlpha);// + (vec4(paintColor * 0.8f * lambert, 1.0) * paintAlpha);
		//fragColor = texture(paintMap, IN.texCoord);
		
		fragColor = mix(fragColor,IN.colour,IN.colour.a);
	
	}
	
	
}