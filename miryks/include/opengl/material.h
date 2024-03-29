#ifndef OPENGL_MATERIAL_H
#define OPENGL_MATERIAL_H

// part of gloom

#include <opengl/renderer.h>

struct Material
{
	static Material *active;

	int id;
	std::string name;
	ShaderSource *src;

	Shader *shader;
	Texture *map, *normalMap, *glowMap;

	// todo what is testing
	// testing is ZBuffer_Test
	// it uses lequal but messes up decals when set to true by default

	bool transparent, doubleSided, blending, ztest, zwrite, decal, tangents, skinning;
	bool dust, modelSpaceNormals, vertexColors;

	int testFunc;

	float opacity, treshold, shininess, glossiness, rotation;
	vec2 offset, repeat, center;
	vec3 color, specular, emissive;

	int bones;
	std::vector<mat4> boneMatrices;
	mat4 bindMatrix;
	mat3 uvTransform;

	struct skin_partition *skin_partition;

	std::string header, defines;

	Material();
	//Material(const Material& other)
	//{
	//	
	//};

	bool prepared = false;
	void RandomColor();
	virtual void Ready();
	virtual void Use();
	static void Unuse(Material *, Material *);

	void composeUvTransform();
	void setUvTransformDirectly(float, float, float, float, float, float, float);

	struct BlendFunc
	{
		int sfactor, dfactor;
	} blendFunc;
};

struct MaterialLand : Material {
	MaterialLand();

	vec2 landOffset;

	vec3 ointments[1089];

	Texture *map2, *map3, *map4, *map5, *map6, *map7, *map8;
	virtual void Use();
	static void Unuse(Material *, Material *);
	virtual void Ready();

};

#endif