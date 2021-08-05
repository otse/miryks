#ifndef OPENGL_SCENE_H
#define OPENGL_SCENE_H

// part of gloom

#include <templates.hpp>

#include <renderer/types.h>

template <typename T>
struct Container
{
	std::vector<T *> ts;
	void Add(T *t)
	{
		vector_safe_add<T *>(t, ts);
	};
	void Remove(T *t)
	{
		vector_safe_remove<T *>(t, ts);
	};
};
struct Scene
{
public:
	Container<DrawGroup> draw_groups;
	Container<PointLight> pointLights;
	Container<SpotLight> spotLights;
	
	//std::vector<DrawGroup *> draw_groups;
	//std::vector<PointLight *> pointLights;

	vec3 ambient;

	Scene();
	~Scene();

	void Clear();
	//void Add(DrawGroup *);
	//void Add(PointLight *);
	//void Remove(DrawGroup *);
	//void Remove(PointLight *);

	void DrawItems();
	void Order();
	void CalcLights();
	void SortLights();
	void BindLights(Shader *);
};

#endif