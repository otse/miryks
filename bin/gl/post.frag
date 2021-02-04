//#version 330 core

out vec4 FragColor;

uniform sampler2D renderedTexture;
uniform float time;

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

	FragColor = diffuseColor;

    FragColor.rgba = vec4(1, 0, 1, 0.5);
}