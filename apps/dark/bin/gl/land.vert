//#version 330 core

// attributes
// values are linearly interpolated between the vertices

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 Uv;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec4 Color;
layout (location = 4) in vec3 Tangent;
layout (location = 5) in vec3 Bitangent;

layout (location = 6) in vec4 SkinIndex;
layout (location = 7) in vec4 SkinWeight;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform mat3 normalMatrix;

uniform mat3 uvTransform;

out vec2 vPos;
out vec2 vUv;
out vec4 vColor;
out vec3 vNormal;
out vec3 vTangent;
out vec3 vBitangent;

out vec4 vLandscape0123;
out vec4 vLandscape4567;

out vec3 vViewPosition;
out mat4 modelView;

out float fogDepth;

// <morphtarget_pars_vertex>
// <skinning_pars_vertex>

#define MAX_BONES 60

#ifndef DONT_USE_SKINNING
	
	uniform mat4 bindMatrix;
	uniform mat4 bindMatrixInverse;

	uniform mat4 boneMatrices[ MAX_BONES ];

	mat4 getBoneMatrix( const in float i ) {

		mat4 bone = boneMatrices[ int(i) ];
		return bone;

	}

#endif

void main()
{
	vec3 objectNormal = vec3( Normal );

	vec3 transformedNormal = normalMatrix * objectNormal;

	//vUv = Uv;
	vPos = Position.xy;
	vUv = ( uvTransform * vec3( Uv, 1 ) ).xy;
	vColor = Color;
	vNormal = normalize(transformedNormal);

	vLandscape0123 = SkinIndex;
	vLandscape4567 = SkinWeight;

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

	// <skinbase_vertex>

	// <skinnormal_vertex>

	vec3 transformed = vec3( Position );

	// yay this fixes messed up skinned normals
	transformedNormal = normalMatrix * objectNormal;
	vNormal = transformedNormal;
	
	modelView = view * model;

	  vec4 mvPosition = modelView * vec4( transformed, 1.0 );
	//vec4 mvPosition = modelView * vec4(vec3(Position), 1.0);
	
	gl_Position = projection * mvPosition;

	vViewPosition = - mvPosition.xyz;

	fogDepth = -mvPosition.z;
}