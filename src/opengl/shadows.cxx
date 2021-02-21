#include <OpenGL/Shadows.h>

// only called with spotlights and directionallights it seems
void LightShadow::updateMatrices(Light *light)
{
	//DirectionalLight *dlight = dynamic_cast<DirectionalLight *>light;

	vec3 position;

	lightPositionWorld = vec3(light->matrix[3]);
	shadowCamera->view[3] = vec4(lightPositionWorld, 1);
	lookTarget = light->target;

	position = shadowCamera->view[3];
	shadowCamera->view = glm::lookAt(position, lookTarget, vec3(0, 0, 1));

	mat4 reverse = glm::inverse(shadowCamera->view);

	projScreenMatrix = shadowCamera->projection * reverse;

	frustum; // set from projection

	matrix = mat4(
		0.5, 0.0, 0.0, 0.5,
		0.0, 0.5, 0.0, 0.5,
		0.0, 0.0, 0.5, 0.5,
		0.0, 0.0, 0.0, 1.0);

	matrix *= shadowCamera->projection;
	matrix *= reverse;
};

void PointLightShadow::updateMatrices(Light *light)
{
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
}

void SpotLightShadow::updateMatrices(Light *light)
{
	/*const camera = this.camera;

		const fov = MathUtils.RAD2DEG * 2 * light.angle * this.focus;
		const aspect = this.mapSize.width / this.mapSize.height;
		const far = light.distance || camera.far;

		if ( fov !== camera.fov || aspect !== camera.aspect || far !== camera.far ) {

			camera.fov = fov;
			camera.aspect = aspect;
			camera.far = far;
			camera.updateProjectionMatrix();

		}

		LightShadow.prototype.updateMatrices.call( this, light );*/

	LightShadow::updateMatrices(light);
}


void ShadowMapRenderer::Render(std::vector<Light *> &lights, Scene *scene, Camera *camera)
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
			shadow->rt = new RenderTarget(shadowMapSize.x, shadowMapSize.y, GL_RGBA, GL_UNSIGNED_BYTE);
			shadow->map = nullptr;
			//shadow->map->name = light.name + '.shadowMap';

			//shadow.camera.updateProjectionMatrix();
		}

		shadow->rt->Bind();

		auto viewportCount = shadow->getViewportCount();

		for (unsigned int vp = 0; vp < viewportCount; vp++)
		{

			auto viewport = shadow->getViewport(vp);

			vec4 _viewport;

			_viewport = glm::vec4(
				viewportSize.x * viewport.x,
				viewportSize.y * viewport.y,
				viewportSize.x * viewport.z,
				viewportSize.y * viewport.w);

			//_state.viewport( _viewport );

			shadow->updateMatrices(light, vp);

			//_frustum = shadow.getFrustum();

			renderObject(scene, camera, shadow->camera, light, this.type);
		}

		shadow->needsUpdate = true;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
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

		lightPositionWorld = vec3(light->matrix[3]);
		shadowCamera->view[3] = vec4(lightPositionWorld, 1);
		lookTarget = light->target;

		position = shadowCamera->view[3];
		shadowCamera->view = glm::lookAt(position, lookTarget, vec3(0, 0, 1));

		mat4 reverse = glm::inverse(shadowCamera->view);

		projScreenMatrix = shadowCamera->projection * reverse;

		frustum; // set from projection

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