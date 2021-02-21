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
	black->color = vec3(0.f);

	ambient = vec3(20.f / 255.f);
	//ambient = vec3(1, 1, 1);
};

Scene::~Scene(){};

void Scene::Clear()
{
	//items.clear();
	//groups2.clear();
}

void Scene::DrawItems()
{
	//CalcLights();
	SortLights();

	for (DrawGroup *drawGroup : drawGroups.ts)
		drawGroup->Draw();
}

void Scene::CalcLights()
{
	//for (PointLight *pl : pointLights.ts)
		//pl->Calc();
}

#include <algorithm>

void Scene::SortLights()
{
	auto sort_distance = [](const Light *a, const Light *b) -> bool {
		if (a->CalcDist() < b->CalcDist())
			return true;
		return false;
	};

	sort(pointLights.ts.begin(), pointLights.ts.end(), sort_distance);
}

void Scene::BindLights(Shader *shader)
{
	shader->SetVec3("ambientLightColor", ambient);

	for (unsigned int i = 0; i < 9; i++)
	{
		PointLight *l = black;

		if (i < pointLights.ts.size())
			l = pointLights.ts[i];

		std::string index = "pointLights[" + std::to_string(i) + "]";

		vec3 position, color;
		position = vec3(l->matrix[3]) * mat3(inverse(camera->view));
		position += vec3(camera->view[3]);
		color = l->color * l->intensity;

		mat3 package;
		package[0] = position;
		package[1] = color;
		package[2][0] = l->distance;
		package[2][1] = l->decay;
		package[2][2] = -1;

		shader->SetMat3((index + ".package").c_str(), package);
	}
}

void Scene::Order()
{
}