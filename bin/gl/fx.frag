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

#define ASH
#define USE_A_HEMISPHERE
#define USE_NORMALMAP
#define USE_SPECULARMAP
#define USE_TANGENT

#ifdef USE_NORMALMAP

	vec2 normalScale = vec2(1.0, 1.0);

#endif

uniform vec3 color;
uniform float opacity;
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

struct BlinnPhongMaterial {
	vec3 diffuseColor;
	vec3 specularColor;
	float specularShininess;
	float specularStrength;
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

struct PointLight {
	vec3 position;
	//float pad3;

	vec3 color;
	//float pad7;
	
	float distance;
	float decay;

	mat3 package;
	//float pad10;
	//float pad11;
};

#define MAX_NUM_TOTAL_LIGHTS 20

layout (std140) uniform pointlights {
	vec4 numPointLights;
	PointLight pointLights2[MAX_NUM_TOTAL_LIGHTS];
}; // semi colon or else

uniform PointLight pointLights[MAX_NUM_TOTAL_LIGHTS];

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

void calcPointLight(
	const in PointLight pointLight, const in GeometricContext geometry, out IncidentLight directLight) {

	vec3 lVector = pointLight.position - geometry.position;
	directLight.direction = normalize( lVector );

	float lightDistance = length( lVector );

	directLight.color = pointLight.color;

	if( lightDistance > 0.0 && pointLight.decay > 0.0 ) {
		directLight.color *= pow( saturate( -lightDistance / pointLight.distance + 1.0 ), pointLight.decay );
	}
	
	directLight.visible = ( directLight.color != vec3( 0.0 ) );

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

void RE_Direct(
	const in IncidentLight directLight, const in GeometricContext geometry, const in BlinnPhongMaterial material, inout ReflectedLight reflectedLight)
{
	float dotNL = saturate( dot( geometry.normal, directLight.direction ) );
	vec3 irradiance = dotNL * directLight.color;

	reflectedLight.directDiffuse +=
		irradiance * RECIPROCAL_PI * material.diffuseColor;

	reflectedLight.directSpecular +=
		irradiance * BRDF_Specular_BlinnPhong( directLight, geometry, material.specularColor, material.specularShininess ) * material.specularStrength;
}

void main()
{
	//vec4 diffuseColor = vec4(color, 1.0);
	vec4 diffuseColor = vec4(1.0, 0.85, 1.0, opacity);

	//vec3 fogColor = vec3(11.0 / 255.0, 6.0 / 255.0, 5.0 / 255.0);
	vec3 fogColor = vec3(6.0 / 255.0, 6.0 / 255.0, 11.0 / 255.0);

	vec3 ambientLightColor = vec3(15.0/255.0, 20.0/255.0, 10.0/255.0) * 1.0;

	float fogNear = 1000.0 / ONE_SKYRIM_UNIT_IN_CM;
	float fogFar = 6000.0 / ONE_SKYRIM_UNIT_IN_CM;
	float fogDensity = 0.000970;

	diffuseColor *= texture2D(map, vUv);
	
	diffuseColor.rgb *= vColor.rgb;

	vec3 normal = normalize( vNormal );

	if (doubleSided) {
		normal = normal * ( float( gl_FrontFacing ) * 2.0 - 1.0 );
	}

	#define BS_EFFECT_SHADER

	#if defined(BS_EFFECT_SHADER) && !defined(COBWEB)

		//diffuseColor.a /= 2.0;

		float dist_x = abs(normal.x);
		float dist_y = abs(normal.y);

		float fade_x = 1.0 - dist_x;
		float fade_y = 1.0;// - dist_y;

		float fades = clamp(fade_x * fade_y, 0.0, 1.0);

		//vec3 fade_normal = normalize(vec3(fade_x, 0, 0) + vec3(0, fade_y, 0));

		float dist = clamp(vViewPosition.z / 200.0, 0.0, 1.0);

		fades *= dist;
		
		//fades = length(fade_normal);

		//fade = fade + fade2 / 2.0;

		//diffuseColor.a = (vColor.a * opacity) * fade;// * fade2;
		diffuseColor.a = vColor.a * fades;

	#endif

	#ifdef USE_TANGENT

		vec3 tangent = normalize( vTangent );
		vec3 bitangent = normalize( vBitangent );

	#endif

	/*#ifdef USE_NORMALMAP

			#ifdef USE_TANGENT

				mat3 vTBN = mat3( tangent, bitangent, normal );
				vec3 mapN = texture2D( normalMap, vUv ).xyz * 2.0 - 1.0;
				// dark flip much needd
				float temp = mapN.r;
				mapN.r = mapN.g;
				mapN.g = temp;
				mapN.xy = normalScale * mapN.xy;
				normal = normalize( vTBN * mapN );

			#endif
	#endif*/

	BlinnPhongMaterial material;
	material.diffuseColor = diffuseColor.rgb;
	material.specularColor = vec3(0.0/255.0, 0.0/255.0, 0.0/255.0);
	material.specularShininess = 0;
	material.specularStrength = 0.0;

	GeometricContext geometry;

	geometry.position = - vViewPosition;
	geometry.normal = normal;
	geometry.viewDir = normalize( vViewPosition );

	IncidentLight directLight;
	ReflectedLight reflectedLight;
	
	PointLight a, b;

	for ( int i = 0; i < 10; i ++ ) {
		a = pointLights[ i ];

		PointLight b = PointLight(
			a.package[0],
			a.package[1],
			a.package[2][0] * 3.0,
			a.package[2][1] * 1.0,
			mat3(0.0));

		calcPointLight( b, geometry, directLight );
		
		RE_Direct( directLight, geometry, material, reflectedLight );
	}

	vec3 irradiance = ambientLightColor * PI;
	//irradiance *= PI; // ?

	/*HemisphereLight hemiLight;
	hemiLight.direction = normalize(vec3(0, 0, -1.0) * mat3(inverse(view)));
	hemiLight.skyColor = vec3(0.0, 0.0, 0.0) * 0.3;
	hemiLight.groundColor = vec3(0.0, 0.0, 0) * 0.3;

	#ifdef USE_A_HEMISPHERE

		float dot_nl = dot( geometry.normal, hemiLight.direction );
		float hemiDiffuseWeight = 0.5 * dot_nl + 0.5;

		irradiance += mix( hemiLight.groundColor, hemiLight.skyColor, hemiDiffuseWeight );

		irradiance *= PI;

	#endif*/



	reflectedLight.indirectDiffuse += irradiance * RECIPROCAL_PI * material.diffuseColor;

	vec3 outgoingLight = 	reflectedLight.directDiffuse +
							reflectedLight.indirectDiffuse +
							reflectedLight.directSpecular +
							reflectedLight.indirectSpecular;

	FragColor = vec4(outgoingLight, diffuseColor.a);

	//RE_IndirectDiffuse( irradiance, geometry, material, reflectedLight );

	// simple fog
	//float fogFactor = smoothstep( fogNear, fogFar, fogDepth );

	// exp2 fog
	float fogFactor = whiteCompliment( exp2( - fogDensity * fogDensity * fogDepth * fogDepth * LOG2 ) );

	FragColor.rgb = mix( FragColor.rgb, fogColor, fogFactor );
}