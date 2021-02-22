#ifndef OPENGL_SCENE_H
#define OPENGL_SCENE_H

// part of gloom

#include <Attic.hpp>

#include <OpenGL/Types.h>

template <typename T>
struct Container
{
	std::vector<T *> ts;
	void Add(T *t)
	{
		VectorAdd<T *>(t, ts);
	};
	void Remove(T *t)
	{
		VectorRemove<T *>(t, ts);
	};
};
struct Scene
{
public:
	Container<DrawGroup> drawGroups;
	Container<PointLight> pointLights;
	Container<SpotLight> spotLights;
	
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