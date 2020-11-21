#ifndef OPENGL_MATERIAL_H
#define OPENGL_MATERIAL_H

#include <opengl/types.h>


struct Material
{
	static Material *active;

	int id;
	std::string name;
	ShaderSource *src;

	Shader *shader;
	Texture *map, *normalMap, *glowMap;

	bool transparent, doubleSided, blending, testing, decal, skinning;
	float opacity, treshold, shininess, glossiness, rotation;
	vec2 offset, repeat, center;
	vec3 color, specular, emissive;

	mat3 uvTransform;

	std::string header;

	Material();

	bool prepared = false;
	void RandomColor();
	void Ready();
	void Use();
	static void Unuse(Material *, Material *);

	void composeUvTransform();
	void setUvTransformDirectly(float, float, float, float, float, float, float);

	struct depth_func_t
	{
		int sfactor, dfactor;
	} depth_func;
};

#endif
