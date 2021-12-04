#ifndef OPENGL_SCENE_H
#define OPENGL_SCENE_H

// part of gloom

#include <templates.hpp>

#include <opengl/renderer.h>

template <typename T>
struct Bluh
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
	DrawGroup *bigGroup;
	
	Bluh<PointLight> pointLights;
	Bluh<SpotLight> spotLights;
	
	//std::vector<DrawGroup *> drawGroups;
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