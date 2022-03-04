#include <templates.hpp>

#include <opengl/scene.h>

#include <opengl/aabb.h>
#include <ogl/scene_graph.hpp>
#include <ogl/scene_graph.hpp>
#include <opengl/material.h>
#include <opengl/shader.h>
#include <opengl/lights.h>

#include <algorithm>

static pointlight *bpoint;
static spotlight *bspot;

Scene::Scene()
{
	bigGroup = new GroupDrawerFlat(new Group, mat4(1.0));

	bpoint = new pointlight;
	bpoint->color = vec3(0.f);

	bspot = new spotlight;
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

	auto EarlyZKills = [](const Group *a, const Group *b) -> bool {
		if (a->GetZ(a->matrixWorld) < b->GetZ(b->matrixWorld))
			return true;
		return false;
	};

	//std::sort(bigGroup->childGroups.begin(), bigGroup->childGroups.end(), EarlyZKills);

#if 0
	auto TransparencyLast = [](const Group *a, const Group *b) -> bool {
		const GroupDrawerFlat *dgfs = dynamic_cast<const GroupDrawerFlat *>(a);
		if (dgfs && dgfs->hasTransparency)
			return false;
		return true;
	};
	std::sort(bigGroup->childGroups.begin(), bigGroup->childGroups.end(), TransparencyLast);
#endif

	bigGroup->DrawChilds(mat4(1.0));
}

void Scene::CalcLights()
{
	//for (pointlight *pl : pointLights.ts)
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
		pointlight *l = bpoint;

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
		spotlight *sl = bspot;

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

		shader->SetVec3((index + ".position").c_str(), position);
		shader->SetVec3((index + ".direction").c_str(), direction);
		shader->SetVec3((index + ".color").c_str(), color);
		shader->SetFloat((index + ".distance").c_str(), sl->distance);
		shader->SetFloat((index + ".coneCos").c_str(), glm::cos(sl->angle));
		shader->SetFloat((index + ".penumbraCos").c_str(), glm::cos(sl->angle * (1.f - sl->penumbra)));
		shader->SetFloat((index + ".decay").c_str(), sl->decay);
	}
}