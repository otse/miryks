#ifndef OPENGL_LIGHTSHADOW_H
#define OPENGL_LIGHTSHADOW_H

#include <OpenGL/Types.h>

#include <OpenGL/Camera.h>
#include <OpenGL/RT.h>

struct ShadowMapRenderer;
struct Light;
struct LightShadow;
struct DirectionalLight;
struct DirectionalLightShadow;
struct PointLight;
struct PointLightShadow;

struct Light
{
	typedef LightShadow Shadow;

	Shadow *shadow;
	mat4 matrix;
	mat4 matrixWorld;
};

struct PointLight : Light
{
	typedef PointLightShadow Shadow;

	//Shadow *shadow;
	float distance, decay;
};

struct DirectionalLight : Light
{
	typedef DirectionalLightShadow Shadow;

	//Shadow *shadow;
	vec3 target;
};

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
	void *mapPass = nullptr;
	mat4 shadowMatrix;
	bool autoUpdate = true;
	bool needsUpdate = false;
	int frustum;
	vec2 frameExtents = vec2(1, 1);
	int viewportCount;
	int viewports;

	mat4 projScreenMatrix;
	vec3 lightPositionWorld;
	vec3 lookTarget;

	int getFrustum() { return 1; };

	int getViewport(int index)
	{
		return 2;
	}
	vec2 getFrameExtents()
	{
		return frameExtents;
	}
};

struct ShadowMapRenderer
{
	vec2 shadowMapSize;
	vec2 viewportSize;
	int *depthMaterials;
	int *distanceMaterials;
	int materialCache;
	int shadowSide = 1;
	bool enabled = true;
	bool autoUpdate = true;
	bool needsUpdate = true;

	void Render(std::vector<Light *> &lights, Scene *scene, Camera *camera)
	{
		glDisable(GL_BLEND);

		// render depth map
		for (int i = 0, il = lights.size(); i < il; i++)
		{
			Light *light = lights[i];
			LightShadow *shadow = light->shadow;

			shadowMapSize = shadow->mapSize;

			vec2 shadowFrameExtents = shadow->getFrameExtents();

			shadowMapSize *= shadowFrameExtents;

			viewportSize = shadow->mapSize;

			if (shadow->map == nullptr)
			{
				shadow->rt  = new RenderTarget( shadowMapSize.x, shadowMapSize.y, GL_RGBA, GL_UNSIGNED_BYTE );
				shadow->map = nullptr;
				//shadow->map->name = light.name + '.shadowMap';

				//shadow.camera.updateProjectionMatrix();
			}

			//if (shadow.)
		}
	};
};

struct DirectionalLightShadow : LightShadow
{
	DirectionalLight *const light;

	DirectionalLightShadow::DirectionalLightShadow(
		DirectionalLight *light) : light(light)
	{
		//shadowCamera = new OrthographicCamera(-5, 5, 5, -5, 0.5, 500));
	}

	void updateMatrices(DirectionalLight *light)
	{
		vec3 position;

		lightPositionWorld = vec3(light->matrixWorld[3]);
		shadowCamera->view[3] = vec4(lightPositionWorld, 1);
		lookTarget = light->target;

		position = shadowCamera->view[3];
		shadowCamera->view = glm::lookAt(position, lookTarget, vec3(0, 0, 1));

		mat4 reverse = glm::inverse(shadowCamera->view);

		projScreenMatrix = shadowCamera->projection * reverse;

		frustum; // set from projection

		shadowMatrix = mat4(
			0.5, 0.0, 0.0, 0.5,
			0.0, 0.5, 0.0, 0.5,
			0.0, 0.0, 0.5, 0.5,
			0.0, 0.0, 0.0, 1.0);

		shadowMatrix *= shadowCamera->projection;
		shadowMatrix *= reverse;
	};
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

	/*updateMatrices( light, viewportIndex = 0 ) {

		const camera = this.camera;
		const shadowMatrix = this.matrix;

		_lightPositionWorld.setFromMatrixPosition( light.matrixWorld );
		camera.position.copy( _lightPositionWorld );

		_lookTarget.copy( camera.position );
		_lookTarget.add( this._cubeDirections[ viewportIndex ] );
		camera.up.copy( this._cubeUps[ viewportIndex ] );
		camera.lookAt( _lookTarget );
		camera.updateMatrixWorld();

		shadowMatrix.makeTranslation( - _lightPositionWorld.x, - _lightPositionWorld.y, - _lightPositionWorld.z );

		_projScreenMatrix.multiplyMatrices( camera.projectionMatrix, camera.matrixWorldInverse );
		this._frustum.setFromProjectionMatrix( _projScreenMatrix );

	}*/
};

#endif