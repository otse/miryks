//#version 330 core

out vec4 FragColor;

uniform mat4 view;
uniform mat4 model;
uniform mat3 normalMatrix;

uniform vec3 ambientLightColor;
in float fogDepth;

in vec2 vUv;
in vec4 vColor;
in vec3 vNormal;
in vec3 vTangent;
in vec3 vBitangent;

in vec3 vViewPosition;
in mat4 modelView;

// texture samplers
uniform sampler2D map;
uniform sampler2D normalMap;
uniform sampler2D glowMap;

//#define PHYSICALLY_CORRECT_LIGHTS

#define USE_A_HEMISPHERE

#define USE_SPECULARMAP

vec2 normalScale = vec2(1.0);

const vec3 sceneSpecular = vec3(0.3, 0.75, 1.0);

uniform vec3 color;
uniform vec3 specular;
uniform vec3 emissive;

uniform float opacity;
uniform float shininess;
uniform float glossiness;
uniform float alphaTest;
uniform bool doubleSided;

#define ONE_SKYRIM_UNIT_IN_CM 1.428

#define LOG2 1.442695

#define saturate(a) clamp( a, 0.0, 1.0 )
#define whiteCompliment(a) ( 1.0 - saturate( a ) )

#define PI 3.14159265359
#define PI2 6.28318530718
#define PI_HALF 1.5707963267949
#define RECIPROCAL_PI 0.31830988618
#define RECIPROCAL_PI2 0.15915494
#define LOG2 1.442695
#define EPSILON 1e-6

struct IncidentLight {
	vec3 color;
	vec3 direction;
	bool visible;
};

struct ReflectedLight {
	vec3 directDiffuse;
	vec3 directSpecular;
	vec3 indirectDiffuse;
	vec3 indirectSpecular;
};

struct GeometricContext {
	vec3 position;
	vec3 normal;
	vec3 viewDir;
};

struct HemisphereLight {
	vec3 direction;
	vec3 skyColor;
	vec3 groundColor;
};

float punctualLightIntensityToIrradianceFactor( const in float lightDistance, const in float cutoffDistance, const in float decayExponent ) {
	if( cutoffDistance > 0.0 && decayExponent > 0.0 ) {
		return pow( saturate( -lightDistance / cutoffDistance + 1.0 ), decayExponent );
	}
	return 1.0;
}

#define NUM_POINT_LIGHTS 8

#if NUM_POINT_LIGHTS > 0

	struct PointLight {
		vec3 position;
		vec3 color;
		float distance;
		float decay;

		mat3 package;
	};

	uniform PointLight pointLights[ NUM_POINT_LIGHTS ];
	
	// directLight is an out parameter as having it as a return value caused compiler errors on some devices

	void getPointDirectLightIrradiance( const in PointLight pointLight, const in GeometricContext geometry, out IncidentLight directLight ) {

		vec3 lVector = pointLight.position - geometry.position;
		directLight.direction = normalize( lVector );

		float lightDistance = length( lVector );

		directLight.color = pointLight.color;
		directLight.color *= punctualLightIntensityToIrradianceFactor( lightDistance, pointLight.distance, pointLight.decay );
		directLight.visible = ( directLight.color != vec3( 0.0 ) );

	}

#endif

#define NUM_SPOT_LIGHTS 2

#if NUM_SPOT_LIGHTS > 0

	struct SpotLight {
		vec3 position;
		vec3 direction;
		vec3 color;
		float distance;
		float decay;
		float coneCos;
		float penumbraCos;

		int shadow;
		float shadowBias;
		float shadowRadius;
		vec2 shadowMapSize;
	};

	uniform SpotLight spotLights[ NUM_SPOT_LIGHTS ];

	// directLight is an out parameter as having it as a return value caused compiler errors on some devices

	void getSpotDirectLightIrradiance( const in SpotLight spotLight, const in GeometricContext geometry, out IncidentLight directLight ) {
		vec3 lVector = spotLight.position - geometry.position;
		directLight.direction = normalize( lVector );
		float lightDistance = length( lVector );
		float angleCos = dot( directLight.direction, spotLight.direction );
		if ( angleCos > spotLight.coneCos ) {
			float spotEffect = smoothstep( spotLight.coneCos, spotLight.penumbraCos, angleCos );
			directLight.color = spotLight.color;
			directLight.color *= spotEffect * punctualLightIntensityToIrradianceFactor( lightDistance, spotLight.distance, spotLight.decay );
			directLight.visible = true;
		} else {
			directLight.color = vec3( 0.0 );
			directLight.visible = false;
		}
	}

#endif


vec3 BRDF_Diffuse_Lambert( const in vec3 diffuseColor ) {

	return RECIPROCAL_PI * diffuseColor;

} // validated

vec3 F_Schlick( const in vec3 specularColor, const in float dotLH ) {
	// Original approximation by Christophe Schlick '94
	// float fresnel = pow( 1.0 - dotLH, 5.0 );
	// Optimized variant (presented by Epic at SIGGRAPH '13)
	// https://cdn2.unrealengine.com/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf
	float fresnel = exp2( ( -5.55473 * dotLH - 6.98316 ) * dotLH );
	return ( 1.0 - specularColor ) * fresnel + specularColor;
} // validated

vec3 F_Schlick_RoughnessDependent( const in vec3 F0, const in float dotNV, const in float roughness ) {
	// See F_Schlick
	float fresnel = exp2( ( -5.55473 * dotNV - 6.98316 ) * dotNV );
	vec3 Fr = max( vec3( 1.0 - roughness ), F0 ) - F0;
	return Fr * fresnel + F0;
}

float G_BlinnPhong_Implicit( /* const in float dotNL, const in float dotNV */ ) {
	// geometry term is (n dot l)(n dot v) / 4(n dot l)(n dot v)
	return 0.25;
}

float D_BlinnPhong( const in float shininess, const in float dotNH ) {
	return RECIPROCAL_PI * ( shininess * 0.5 + 1.0 ) * pow( dotNH, shininess );
}

vec3 BRDF_Specular_BlinnPhong( const in IncidentLight incidentLight, const in GeometricContext geometry, const in vec3 specularColor, const in float shininess ) {
	vec3 halfDir = normalize( incidentLight.direction + geometry.viewDir );
	//float dotNL = saturate( dot( geometry.normal, incidentLight.direction ) );
	//float dotNV = saturate( dot( geometry.normal, geometry.viewDir ) );
	float dotNH = saturate( dot( geometry.normal, halfDir ) );
	float dotLH = saturate( dot( incidentLight.direction, halfDir ) );
	vec3 F = F_Schlick( specularColor, dotLH );
	float G = G_BlinnPhong_Implicit( /* dotNL, dotNV */ );
	float D = D_BlinnPhong( shininess, dotNH );
	return F * ( G * D );
} // validated

struct BlinnPhongMaterial {
	vec3 diffuseColor;
	vec3 specularColor;
	float specularShininess;
	float specularStrength;
};

void RE_Direct_BlinnPhong( const in IncidentLight directLight, const in GeometricContext geometry, const in BlinnPhongMaterial material, inout ReflectedLight reflectedLight ) {
	float dotNL = saturate( dot( geometry.normal, directLight.direction ) );
	vec3 irradiance = dotNL * directLight.color;
	#ifndef PHYSICALLY_CORRECT_LIGHTS
		irradiance *= PI; // punctual light
	#endif
	reflectedLight.directDiffuse += irradiance * BRDF_Diffuse_Lambert( material.diffuseColor );
	reflectedLight.directSpecular += irradiance * BRDF_Specular_BlinnPhong( directLight, geometry, material.specularColor, material.specularShininess ) * material.specularStrength;
}

void RE_IndirectDiffuse_BlinnPhong( const in vec3 irradiance, const in GeometricContext geometry, const in BlinnPhongMaterial material, inout ReflectedLight reflectedLight ) {
	reflectedLight.indirectDiffuse += irradiance * BRDF_Diffuse_Lambert( material.diffuseColor );
}

#define RE_Direct				RE_Direct_BlinnPhong
#define RE_IndirectDiffuse		RE_IndirectDiffuse_BlinnPhong

#define Material_LightProbeLOD( material )	(0)

void main()
{
	//vec4 diffuseColor = vec4(color, 1.0);
	vec4 diffuseColor = vec4(color, opacity);

	// sulfur mine
	//vec3 fogColor = vec3(5.0 / 255.0, 4.0 / 255.0, 3.0 / 255.0);

	// blue
	vec3 fogColor = vec3(6.0 / 255.0, 9.0 / 255.0, 11.0 / 255.0) * 1.0;

	float fogNear = 1000.0 / ONE_SKYRIM_UNIT_IN_CM;
	float fogFar = 15000.0 / ONE_SKYRIM_UNIT_IN_CM;
	float fogDensity = 0.0011; // 0.0022 for dark dungeon 0.0011 for light

	vec3 totalEmissiveRadiance = emissive;

	diffuseColor *= texture2D(map, vUv);

	#ifdef DONT_USE_DIFFUSE_MAP

		diffuseColor.rgb = vec3(1);

	#endif

	//#ifndef VERTEX_ALPHA
	//vColor.a = 1.0;
	//#endif

	#ifndef DONT_USE_VERTEX_COLOR
		
		//#ifdef VERTEX_ALPHA
		#ifndef TREE_ANIM
			diffuseColor.rgba *= vColor.rgba;
		#else
			diffuseColor.rgb *= vColor.rgb;
		#endif
	
	#endif

	//#ifndef DONT_USE_SKINNING

	//	diffuseColor.rgba = vec4(0, 1, 1, 1);

	//#endif
	
	if (diffuseColor.a <= alphaTest)
		//diffuseColor.r = 1.0;
		discard;

	vec3 normal = normalize( vNormal );

	if (doubleSided) {
		normal = normal * ( float( gl_FrontFacing ) * 2.0 - 1.0 );
	}

	float specularStrength = 0;//glossiness / 999;

	#ifndef DONT_USE_SPECULAR_MAP

		vec4 texelSpecular = texture2D( normalMap, vUv );
		specularStrength = texelSpecular.a * 1.0;

	#endif

	#ifdef USE_TANGENT

		vec3 tangent = normalize( vTangent );
		vec3 bitangent = normalize( vBitangent );

		if (doubleSided) {
			tangent = tangent * ( float( gl_FrontFacing ) * 2.0 - 1.0 );
			bitangent = bitangent * ( float( gl_FrontFacing ) * 2.0 - 1.0 );
		}

	#endif

	#ifdef TREE_ANIM

		specularStrength /= 2.0;
		normalScale /= 3.0;

	#endif
	
	#ifndef DONT_USE_NORMAL_MAP

			// diffuseColor.rgb *= vec3(1, 0, 0);

			#ifdef USE_TANGENT

				mat3 vTBN = mat3( tangent, bitangent, normal );
				vec3 mapN = texture2D( normalMap, vUv ).xyz * 2.0 - 1.0;
				// dark flip much needd
				float temp = mapN.r;
				mapN.r = mapN.g;
				mapN.g = temp;
				mapN.xy = normalScale * mapN.xy;
				normal = normalize( vTBN * mapN );

				// diffuseColor.rgb *= vec3(2, 0, 0);

			#endif

			#ifdef MODEL_SPACE_NORMALS

				// x
				vec3 mapN = texture2D( normalMap, vUv ).xyz * 2.0 - 1.0;
				//normal = texture2D( normalMap, vUv ).xyz * 2.0 - 1.0;

				// dark flip much needd
				float temp = mapN.b;
				mapN.b = mapN.g;
				mapN.g = temp;

				if (doubleSided)
					mapN = mapN * ( float( gl_FrontFacing ) * 2.0 - 1.0 );

				normal = normalize( normalMatrix * mapN );

				// diffuseColor.rgb = vec3(1, 1, 5);

			#endif

	#endif

	#ifndef DONT_USE_GLOW_MAP

		vec4 emissiveColor = texture2D( glowMap, vUv );

		totalEmissiveRadiance *= emissiveColor.rgb;
	#else
		totalEmissiveRadiance = vec3(0);
	#endif

	// #define DONT_USE_DUST
	#ifndef DONT_USE_DUST

	#ifndef TREE_ANIM

		vec3 normalDust = normalize(normal) * mat3(view);

		vec3 dust = vec3(0.15, 0.15, 0.15);
		
		if (normalDust.z > 0.83) { // .88
			diffuseColor.rgb = mix(dust, diffuseColor.rgb, 0.075);
			specularStrength /= 3.0;
		}

	#endif

	#endif

	BlinnPhongMaterial material;
	material.diffuseColor = diffuseColor.rgb;
	//material.specularColor = vec3(17.0/255.0, 17.0/255.0, 17.0/255.0);
	material.specularColor = specular + vec3(0.0, 0.8, 0.5);
	material.specularColor /= 4.0;
	//material.specularColor = vec3(127.0/255.0, 127.0/255.0, 127.0/255.0);
	material.specularShininess = glossiness / 2000 * 100; // shininess 1000 = shiny, 3000 = matt, 2000 = default
	material.specularStrength = specularStrength;

	GeometricContext geometry;

	geometry.position = - vViewPosition;
	geometry.normal = normal;
	geometry.viewDir = normalize( vViewPosition );

	IncidentLight directLight;
	ReflectedLight reflectedLight;
	
	#if ( NUM_POINT_LIGHTS > 0 )

		PointLight a;
		PointLight b;

		for ( int i = 0; i < NUM_POINT_LIGHTS; i ++ ) {
			a = pointLights[ i ];

			PointLight b = PointLight(
				a.package[0],
				a.package[1] * 1.0,
				a.package[2][0],
				a.package[2][1],
				mat3(0.0));

			getPointDirectLightIrradiance( b, geometry, directLight );
			
			RE_Direct( directLight, geometry, material, reflectedLight );
		}

	#endif

	#if ( NUM_SPOT_LIGHTS > 0 )
		SpotLight spotLight;
		#if defined( USE_SHADOWMAP ) && NUM_SPOT_LIGHT_SHADOWS > 0
			SpotLightShadow spotLightShadow;
		#endif
		//#pragma unroll_loop_start
		for ( int i = 0; i < NUM_SPOT_LIGHTS; i ++ ) {
			spotLight = spotLights[ i ];
			spotLight.color *= 2.0;
			spotLight.distance *= 20.0;
			//spotLight.direction = spotLight.direction;
			spotLight.coneCos = cos(PI / 7.0);
			spotLight.penumbraCos = cos(spotLight.coneCos * (1.0 - 1.0));
			//spotLight.direction = vec3(0, 0, 1) * mat3(inverse(view));
			getSpotDirectLightIrradiance( spotLight, geometry, directLight );
			#if defined( USE_SHADOWMAP ) && ( UNROLLED_LOOP_INDEX < NUM_SPOT_LIGHT_SHADOWS )
				spotLightShadow = spotLightShadows[ i ];
				directLight.color *= all( bvec2( directLight.visible, receiveShadow ) ) ? getShadow( spotShadowMap[ i ], spotLightShadow.shadowMapSize, spotLightShadow.shadowBias, spotLightShadow.shadowRadius, vSpotShadowCoord[ i ] ) : 1.0;
			#endif
			RE_Direct( directLight, geometry, material, reflectedLight );
		}
		//#pragma unroll_loop_end
	#endif

	HemisphereLight hemiLight;
	hemiLight.direction = normalize(vec3(0, 0, -1.0) * mat3(inverse(view)));
	hemiLight.skyColor = vec3(0.02, 0.02, 0.06) * 0.22;
	hemiLight.groundColor = vec3(0.03, 0.05, 0.04) * 2.0;

	// ambientLightColor
	// 30 / 255 for dark dungeon
	vec3 localAmbient = ambientLightColor; // vec3(20.0 / 255.0);
	vec3 irradiance = localAmbient * PI;

	#define DONT_USE_A_HEMISPHERE
	#ifndef DONT_USE_A_HEMISPHERE

		float dot_nl = dot( geometry.normal, hemiLight.direction );
		float hemiDiffuseWeight = 0.5 * dot_nl + 0.5;

		vec3 irradiance2 = mix( hemiLight.groundColor, hemiLight.skyColor, hemiDiffuseWeight );

		// #ifndef physically correct lights
		irradiance2 *= PI;

		irradiance += irradiance2;

	#endif

	reflectedLight.indirectDiffuse += irradiance * RECIPROCAL_PI * material.diffuseColor;

	vec3 outgoingLight = 	reflectedLight.directDiffuse +
							reflectedLight.indirectDiffuse +
							reflectedLight.directSpecular +
							reflectedLight.indirectSpecular +
							totalEmissiveRadiance;

	FragColor = vec4(outgoingLight, diffuseColor.a);

	//RE_IndirectDiffuse( irradiance, geometry, material, reflectedLight );

	// simple fog
	//float fogFactor = smoothstep( fogNear, fogFar, fogDepth );

	// exp2 fog
	float fogFactor = whiteCompliment( exp2( - fogDensity * fogDensity * fogDepth * fogDepth * LOG2 ) );

	FragColor.rgb = mix( FragColor.rgb, fogColor, fogFactor );
}