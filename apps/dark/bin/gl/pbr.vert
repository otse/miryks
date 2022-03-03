//#version 330 core

#define PHYSICAL

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 Uv;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec4 Color;
layout (location = 4) in vec3 Tangent;
layout (location = 5) in vec3 Bitangent;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform mat3 normalMatrix;

uniform mat3 uvTransform;

out vec2 vUv;
out vec4 vColor;
out vec3 vNormal;
out vec3 vTangent;
out vec3 vBitangent;

out vec3 vViewPosition;
out mat4 modelView;

varying float fogDepth;

#define USE_TANGENT

void main() {

	vec3 objectNormal = vec3( Normal );
	vec3 transformedNormal = normalMatrix * objectNormal;

	vUv = ( uvTransform * vec3( Uv, 1 ) ).xy;
	vColor = Color;
	vNormal = normalize(transformedNormal);

	vec4 tangent = vec4(Tangent, 1.0);

	#ifdef USE_TANGENT

		vec3 objectTangent = vec3( Tangent.xyz );
		vec3 objectBitangent = vec3( Bitangent.xyz );

		vec3 transformedTangent = normalMatrix * objectTangent;
		vec3 transformedBitangent = normalMatrix * objectBitangent;

		vTangent = normalize( transformedTangent );
		vBitangent = normalize( cross( vNormal, vTangent ) * tangent.w );
		//vBitangent = normalize( transformedBitangent );

	#endif

	modelView = view * model;
	
	gl_Position = projection * modelView * vec4(Position, 1.0);

	vec4 mvPosition = modelView * vec4(vec3(Position), 1.0);

	vViewPosition = - mvPosition.xyz;

	fogDepth = -mvPosition.z;

}