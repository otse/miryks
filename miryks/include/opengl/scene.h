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
	GroupDrawer *bigGroup;
	
	Bluh<pointlight> pointLights;
	Bluh<spotlight> spotLights;
	
	//std::vector<GroupDrawer *> drawGroups;
	//std::vector<pointlight *> pointLights;

	vec3 ambient;

	Scene();
	~Scene();

	void Clear();
	//void Add(GroupDrawer *);
	//void Add(pointlight *);
	//void Remove(GroupDrawer *);
	//void Remove(pointlight *);

	void DrawItems();
	void Order();
	void CalcLights();
	void SortLights();
	void BindLights(Shader *);
};

#endif