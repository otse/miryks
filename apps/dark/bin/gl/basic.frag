//#version 330 core

out vec4 FragColor;

uniform mat4 view;
uniform mat4 model;
uniform mat3 normalMatrix;

uniform vec3 ambientLightColor;
in float fogDepth;

in vec2 vUv;
in vec4 vColor;

in vec3 vViewPosition;
in mat4 modelView;

uniform vec3 color;
uniform float opacity;

#define ONE_SKYRIM_UNIT_IN_CM 1.428

#define LOG2 1.442695

#define saturate(a) clamp( a, 0.0, 1.0 )
#define whiteCompliment(a) ( 1.0 - saturate( a ) )


void main()
{
	vec4 diffuseColor = vec4(color, opacity);

	vec3 fogColor = vec3(6.0 / 255.0, 6.0 / 255.0, 11.0 / 255.0);

	float fogNear = 1000.0 / ONE_SKYRIM_UNIT_IN_CM;
	float fogFar = 6000.0 / ONE_SKYRIM_UNIT_IN_CM;
	float fogDensity = 0.000970;
	
	diffuseColor.rgba *= vColor.rgba;

	FragColor = diffuseColor;

	// exp2 fog
	float fogFactor = whiteCompliment( exp2( - fogDensity * fogDensity * fogDepth * fogDepth * LOG2 ) );

	FragColor.rgb = mix( FragColor.rgb, fogColor, fogFactor );
}