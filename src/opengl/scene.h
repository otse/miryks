#ifndef OPENGL_SCENE_H
#define OPENGL_SCENE_H

#include "types.h"

struct Scene
{
public:
	std::vector<PointLight *> pointlights;
	std::vector<Renderable *> renderables;
	std::vector<RenderItem> objects;
	std::vector<Group *> classics;
	
	vec3 ambient;

	Scene();
	~Scene();

	void Clear();
	void Add(Renderable *);
	void Add(PointLight *);
	void Remove(Renderable *);
	void Remove(PointLight *);

	void DrawItems();
	void Order();
	void CalcLights();
	void SortLights();
	void BindLights(Shader *);
};

#endif