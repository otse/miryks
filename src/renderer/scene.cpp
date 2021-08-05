#include <templates.hpp>

#include <renderer/scene.h>

#include <renderer/Aabb.h>
#include <renderer/group.h>
#include <renderer/drawgroup.h>
#include <renderer/material.h>
#include <renderer/shader.h>
#include <renderer/lights.h>

#include <algorithm>

static PointLight *bpoint;
static SpotLight *bspot;

Scene::Scene()
{
	bpoint = new PointLight;
	bpoint->color = vec3(0.f);

	bspot = new SpotLight;
	bspot->color = vec3(0.f);

	ambient = vec3(50.f / 255.f);
	//ambient = vec3(1, 1, 1);
};

Scene::~Scene(){};

void Scene::Clear()
{
	//items.clear();
	//groups2.clear();
}

void Scene::Order()
{
}

void Scene::DrawItems()
{
	//CalcLights();
	SortLights();

	auto EarlyZKills = [](const DrawGroup *a, const DrawGroup *b) -> bool {
		if (a->GetZ() < b->GetZ())
			return true;
		return false;
	};

	std::sort(draw_groups.ts.begin(), draw_groups.ts.end(), EarlyZKills);

	for (DrawGroup *draw_group : draw_groups.ts)
		draw_group->Draw();
}

void Scene::CalcLights()
{
	//for (PointLight *pl : pointLights.ts)
		//pl->Calc();
}


void Scene::SortLights()
{
	auto DistanceSort = [](const Light *a, const Light *b) -> bool {
		if (a->CalcDist() < b->CalcDist())
			return true;
		return false;
	};

	sort(pointLights.ts.begin(), pointLights.ts.end(), DistanceSort);
}

void Scene::BindLights(Shader *shader)
{
	shader->SetVec3("ambientLightColor", ambient);

	for (unsigned int i = 0; i < 6; i++)
	{
		PointLight *l = bpoint;

		if (i < pointLights.ts.size())
			l = pointLights.ts[i];

		std::string index = "pointLights[" + std::to_string(i) + "]";

		vec3 position, color;
		position = vec3(l->matrix[3]) * mat3(inverse(cameraCur->view));
		position += vec3(cameraCur->view[3]);
		color = l->color * l->intensity;

		mat3 package;
		package[0] = position;
		package[1] = color;
		package[2][0] = l->distance;
		package[2][1] = l->decay;
		package[2][2] = -1;

		shader->SetMat3((index + ".package").c_str(), package);
	}

	for (unsigned int i = 0; i < 2; i++)
	{
		SpotLight *sl = bspot;

		if (i < spotLights.ts.size())
			sl = spotLights.ts[i];

		std::string index = "spotLights[" + std::to_string(i) + "]";

		vec3 position, direction, color;
		position = vec3(sl->matrix[3]) * mat3(inverse(cameraCur->view));
		position += vec3(cameraCur->view[3]);

		mat4 ma = sl->matrix;
		ma = rotate(ma, -pif/2, vec3(0, 1, 0));

		vec3 dir = normalize(vec3(mat3(ma)[2]));
		dir = normalize(dir);

		direction = dir * mat3(inverse(cameraCur->view));
		//printf("sldir %s\n", glm::to_string(direction));

		//direction = glm::normalize(glm::vec3(glm::inverse(mat3(sl->matrix * mat3(cameraCur->view))[2]));
		color = sl->color * sl->intensity;

		float moo = glm::cos(sl->angle);

		static bool pinted = false;

		if (!pinted) {
			pinted = true;
			printf("moo %f\n", moo);
		} 

		shader->SetVec3((index + ".position").c_str(), position);
		shader->SetVec3((index + ".direction").c_str(), direction);
		shader->SetVec3((index + ".color").c_str(), color);
		shader->SetFloat((index + ".distance").c_str(), sl->distance);
		shader->SetFloat((index + ".coneCos").c_str(), glm::cos(sl->angle));
		shader->SetFloat((index + ".penumbraCos").c_str(), glm::cos(sl->angle * (1.f - sl->penumbra)));
		shader->SetFloat((index + ".decay").c_str(), sl->decay);
	}
}