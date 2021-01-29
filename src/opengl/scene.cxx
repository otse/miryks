#include <attic.hpp>

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

void Scene::Add(PointLight *pl) {
	gloom::add<PointLight *>(pl, pointlights);
}

void Scene::Remove(PointLight *pl) {
	gloom::remove<PointLight *>(pl, pointlights);
}

void Scene::Add(DrawGroup *drawGroup) {
	gloom::add<DrawGroup *>(drawGroup, drawGroups);
}

void Scene::Remove(DrawGroup *drawGroup) {
	gloom::remove<DrawGroup *>(drawGroup, drawGroups);
}

void Scene::DrawItems()
{
	CalcLights();
	SortLights();

	for (DrawGroup *drawGroup : drawGroups)
		drawGroup->Draw();
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