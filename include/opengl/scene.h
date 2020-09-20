#ifndef OPENGL_SCENE_H
#define OPENGL_SCENE_H

#include <opengl/types.h>

struct Scene
{
public:
	std::vector<DrawGroup *> drawGroups;
	std::vector<PointLight *> pointlights;
	
	vec3 ambient;

	Scene();
	~Scene();

	void Clear();
	void Add(DrawGroup *);
	void Remove(DrawGroup *);
	void Add(PointLight *);
	void Remove(PointLight *);

	void DrawItems();
	void Order();
	void CalcLights();
	void SortLights();
	void BindLights(Shader *);
};

#endif