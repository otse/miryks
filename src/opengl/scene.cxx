#include <Attic.hpp>

#include <OpenGL/Scene.h>

#include <OpenGL/Aabb.h>
#include <OpenGL/Group.h>
// #include <OpenGL/renderable.h>
#include <OpenGL/Material.h>
#include <OpenGL/Shader.h>
#include <OpenGL/Lights.h>

static PointLight *black;

Scene::Scene()
{
	black = new PointLight;
	black->color = vec3(0);
	black->distance = 0;
	black->intensity = 0;

	ambient = vec3(20.f / 255.f);
	//ambient = vec3(1, 1, 1);
};

Scene::~Scene(){};

void Scene::Clear()
{
	//items.clear();
	//groups2.clear();
}

/*void Scene::Add(PointLight *pl) 			{ VectorAdd<PointLight *>(pl, pointLights); }
void Scene::Add(DrawGroup *drawGroup) 		{ VectorAdd<DrawGroup *>(drawGroup, drawGroups); }
void Scene::Remove(PointLight *pl) 			{ VectorRemove<PointLight *>(pl, pointLights); }
void Scene::Remove(DrawGroup *drawGroup) 	{ VectorRemove<DrawGroup *>(drawGroup, drawGroups); }*/

void Scene::DrawItems()
{
	CalcLights();
	SortLights();

	for (DrawGroup *drawGroup : drawGroups.ts)
		drawGroup->Draw();
}

void Scene::CalcLights()
{
	for (PointLight *pl : pointLights.ts)
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

	sort(pointLights.ts.begin(), pointLights.ts.end(), sort_distance);
}

void Scene::BindLights(Shader *shader)
{
	shader->SetVec3("ambientLightColor", ambient);

	for (unsigned i = 0; i < 9; i++)
	{
		PointLight *l = black;

		if (i < pointLights.ts.size())
			l = pointLights.ts[i];

		std::string index = "pointLights[" + std::to_string(i) + "]";

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