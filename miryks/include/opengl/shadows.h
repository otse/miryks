#ifndef OPENGL_SHADOWS_H
#define OPENGL_SHADOWS_H

// part of gloom

#include <opengl/renderer.h>

#include <opengl/camera.h>
#include <opengl/lights.h>
#include <opengl/rendertarget.h>

// https://github.com/mrdoob/three.js/blob/master/src/lights/LightShadow.js

struct LightShadow
{
	Camera *shadowCamera;
	mat4 matrix;
	float bias = 0;
	float normalBias = 0;
	float radius = 1;
	vec2 mapSize = vec2(512, 512);
	RenderTarget *rt = nullptr;
	Texture *map = nullptr;
	bool autoUpdate = true;
	bool needsUpdate = false;
	vec2 frameExtents = vec2(1, 1);
	std::vector<glm::vec4> viewports = {glm::vec4(0, 0, 1, 1)};
	mat4 projScreenMatrix;
	vec3 lightPositionWorld;
	vec3 lookTarget;

	LightShadow();

	// only called with spotlights and directionallights it seems
	void updateMatrices(Light *light)
	{
		vec3 position;

		lightPositionWorld = vec3(light->matrix[3]);
		shadowCamera->view[3] = vec4(lightPositionWorld, 1);
		lookTarget = light->target;

		position = shadowCamera->view[3];
		shadowCamera->view = glm::lookAt(position, lookTarget, vec3(0, 0, 1));

		mat4 reverse = glm::inverse(shadowCamera->view);

		projScreenMatrix = shadowCamera->projection * reverse;

		matrix = mat4(
			0.5, 0.0, 0.0, 0.5,
			0.0, 0.5, 0.0, 0.5,
			0.0, 0.0, 0.5, 0.5,
			0.0, 0.0, 0.0, 1.0);

		matrix *= shadowCamera->projection;
		matrix *= reverse;
	};
};
struct PointLightShadow : LightShadow
{
	PointLightShadow(){

	};

	void updateMatrices(Light *light)
	{
	}
};
struct SpotLightShadow : LightShadow
{
	SpotLightShadow(){

	};
};
struct DirectionalLightShadow : LightShadow
{
	DirectionalLightShadow(){

	};
};

struct ShadowMapRenderer
{
	vec2 mapSize;
	vec2 viewportSize;
	vec4 viewport;
	int *depthMaterials;
	int *distanceMaterials;
	int materialCache;
	int shadowSide = 1;
	bool enabled = true;
	bool autoUpdate = true;
	bool needsUpdate = true;

	void Render(std::vector<Light *> &shadowLights, Scene *scene, Camera *camera)
	{
		glDisable(GL_BLEND);

		// render depth map
		for (int i = 0, il = shadowLights.size(); i < il; i++)
		{
			Light *light = shadowLights[i];
			LightShadow *shadow = light->shadow;

			mapSize = shadow->mapSize;

			vec2 frameExtents = shadow->frameExtents;

			mapSize *= frameExtents;

			viewportSize = shadow->mapSize;

			if (shadow->map == nullptr)
			{
				shadow->rt = new RenderTarget(mapSize.x, mapSize.y, GL_RGBA, GL_UNSIGNED_BYTE);
				shadow->map = nullptr;
				//shadow->map->name = light.name + '.shadowMap';

				//shadow.camera.updateProjectionMatrix();
			}

			shadow->rt->Bind();

			auto viewport = shadow->viewports[0];

			vec4 _viewport;

			_viewport = glm::vec4(
				viewportSize.x * viewport.x,
				viewportSize.y * viewport.y,
				viewportSize.x * viewport.z,
				viewportSize.y * viewport.w);

			//_state.viewport( _viewport );

			shadow->updateMatrices(light);

			//_frustum = shadow.getFrustum();

			// renderObject(scene, camera, shadow->shadowCamera, light, this->type);

			shadow->needsUpdate = true;

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	};
};

#endif