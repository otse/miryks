#ifndef OPENGL_SCENE_H
#define OPENGL_SCENE_H

#include <opengl/types.h>

struct Scene
{
public:
	std::vector<Group *> groups;
	std::vector<PointLight *> pointlights;
	
	vec3 ambient;

	Scene();
	~Scene();

	void Clear();
	void Add(Group *);
	void Remove(Group *);
	void Add(PointLight *);
	void Remove(PointLight *);

	void DrawItems();
	void Order();
	void CalcLights();
	void SortLights();
	void BindLights(Shader *);
};

#endif