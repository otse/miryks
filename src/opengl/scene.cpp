#include <opengl/scene.h>

#include <opengl/aabb.h>
#include <opengl/group.h>
#include <opengl/renderable.h>
#include <opengl/material.h>
#include <opengl/shader.h>
#include <opengl/pointlight.h>


static PointLight *black;

Scene::Scene()
{
	black = new PointLight;
	black->color = vec3(0);
	black->distance = 0;
	black->intensity = 0;

	ambient = vec3(1.0);
	//ambient = vec3(1, 1, 1);
};

Scene::~Scene(){};

void Scene::Clear()
{
	//items.clear();
	//groups2.clear();
}

template <typename T>
bool add_nullable(T t, std::vector<T> &v)
{
	if (t == nullptr)
		return false;
	std::vector<T>::iterator has = std::find(v.begin(), v.end(), t);
	if (has == v.end())
	{
		// We don't have this yet, add it
		v.push_back(t);
		return true;
	}
	return false;
}

template <typename T>
bool remove_nullable(T t, std::vector<T> &v)
{
	if (t == nullptr)
		return false;
	std::vector<T>::iterator has = std::find(v.begin(), v.end(), t);
	if (has != v.end())
	{
		// We have this, erase it
		v.erase(has);
		return true;
	}
	return false;
}

void Scene::Add(PointLight *pl) {
	add_nullable<PointLight *>(pl, pointlights);
}
void Scene::Remove(PointLight *pl) {
	remove_nullable<PointLight *>(pl, pointlights);
}
void Scene::Add(Renderable *rb) {
	if (add_nullable<Renderable *>(rb, renderables))
	{
		objects.insert(objects.end(), rb->objects.begin(), rb->objects.end());
	}
}
void Scene::Remove(Renderable *rb) {
	if (remove_nullable<Renderable *>(rb, renderables))
	{
		auto it = objects.begin();
		for (; it != objects.end();)
		{
			if ((*it).renderable == rb)
			{
				it = objects.erase(it);
			}
			else
				++it;
		}
	}
}

void Scene::DrawItems()
{
	CalcLights();
	SortLights();

	for (RenderItem &render_item : objects)
	{
		//render_item.Draw();
	}

	//return;
	for (Renderable *renderable : renderables)
	{
		renderable->DrawClassic();
	}
}

void Scene::CalcLights()
{
	for (PointLight *pl : pointlights)
		pl->Calc();
}

#include <algorithm>

void Scene::SortLights()
{
	auto sort_distance = [](const PointLight *a, const PointLight *b) -> bool {
		if (a->GetDist() < b->GetDist())
			return true;
		return false;
	};

	sort(pointlights.begin(), pointlights.end(), sort_distance);
}

void Scene::BindLights(Shader *shader)
{
	shader->SetVec3("ambientLightColor", ambient);

	for (unsigned i = 0; i < 9; i++)
	{
		PointLight *l = black;

		if (i < pointlights.size())
			l = pointlights[i];

		string index = "pointLights[" + std::to_string(i) + "]";

		mat3 package;
		package[0] = l->position_;
		package[1] = l->color_;
		package[2][0] = l->distance;
		package[2][1] = l->decay;
		package[2][2] = -1;

		shader->SetMat3((index + ".package").c_str(), package);
	}
}

void Scene::Order()
{
}