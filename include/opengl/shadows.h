#ifndef OPENGL_SHADOWS_H
#define OPENGL_SHADOWS_H

// part of gloom

#include <OpenGL/Types.h>

#include <OpenGL/Camera.h>
#include <OpenGL/Lights.h>
#include <OpenGL/RT.h>

// https://github.com/mrdoob/three.js/blob/master/src/lights/LightShadow.js

struct LightShadow
{
	Camera *shadowCamera;
	float bias = 0;
	float normalBias = 0;
	float radius = 1;
	vec2 mapSize = vec2(512, 512);
	RenderTarget *rt = nullptr;
	Texture *map = nullptr;
	void *mapPass = nullptr; // for vsmshadowmap ?
	mat4 matrix;
	bool autoUpdate = true;
	bool needsUpdate = false;
	int frustum = -1;
	vec2 frameExtents = vec2(1, 1);
	int viewportCount = 1;
	std::vector<glm::vec4> viewports = {glm::vec4(0, 0, 1, 1)};

	mat4 projScreenMatrix;
	vec3 lightPositionWorld;
	vec3 lookTarget;

	int getFrustum() { return 1; };

	int getViewportCount()
	{
		return viewportCount;
	}

	glm::vec4 getViewport(int index)
	{
		return viewports[index];
	}

	vec2 getFrameExtents()
	{
		return frameExtents;
	}

	virtual void updateMatrices(Light *);
};

struct ShadowMapRenderer
{
	vec2 shadowMapSize;
	vec2 viewportSize;
	vec4 viewport;
	int *depthMaterials;
	int *distanceMaterials;
	int materialCache;
	int shadowSide = 1;
	bool enabled = true;
	bool autoUpdate = true;
	bool needsUpdate = true;

	void Render(std::vector<Light *> &lights, Scene *scene, Camera *camera);
};

struct DirectionalLightShadow : LightShadow
{
	DirectionalLight *const light;

	DirectionalLightShadow::DirectionalLightShadow(
		DirectionalLight *light) : light(light)
	{
		//shadowCamera = new OrthographicCamera(-5, 5, 5, -5, 0.5, 500));
	}
};

struct SpotLightShadow : LightShadow
{
	SpotLightShadow();

	virtual void updateMatrices(Light *);
};

struct PointLightShadow : LightShadow
{
	PointLight *const light;

	PointLightShadow::PointLightShadow(
		PointLight *light) : light(light)
	{
		// new PerspectiveCamera( 90, 1, 0.5, 500 )

		// this._frameExtents = new Vector2( 4, 2 );

		// this._viewportCount = 6;
	}

	virtual void updateMatrices(Light *);
};

#endif