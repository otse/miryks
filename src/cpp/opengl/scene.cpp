
#include "scene"

#include "aabb"
#include "group"
#include "material"
#include "shader"
#include "pointlight"


static PointLight *black;

Scene::Scene()
{
	black = new PointLight;
	black->color = vec3(0);
	black->distance = 0;
	black->intensity = 0;

	//ambient = vec3(100.f / 255.f, 100.f / 255.f, 100.f / 255.f);
	ambient = vec3(1, 1, 1);
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

void Scene::Add(PointLight *l)
{
	add_nullable<PointLight *>(l, pointlights);
}

void Scene::Remove(PointLight *l)
{
	remove_nullable<PointLight *>(l, pointlights);
}

void Scene::Add(Group *gr)
{
	add_nullable<Group *>(gr, groups);
}

void Scene::DrawItems()
{
	CalcLights();
	SortLights();

	//for (Rent &item : items)
	//{
	//	item.draw();
	//}

	for (Group *gr : groups)
	{
		gr->DrawClassic(mat4(1));
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