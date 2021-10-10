#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <renderer/renderer.h>

#include <renderer/camera.h>
#include <renderer/scene.h>
#include <renderer/shader.h>
#include <renderer/rendertarget.h>
#include <renderer/drawgroup.h>

#include <renderer/material.h>
#include <renderer/texture.h>

int width = 1920;
int height = 1080;
int fps = 0;
float delta = 0;

Camera *cameraCur = nullptr;
Scene *sceneCur = nullptr, *sceneDef = nullptr;

FirstPersonCamera *personCam = nullptr;
ViewerCamera *viewerCam = nullptr;

RenderTarget *renderTargetDef = nullptr;

RenderSettings renderSettings;

bool checkGlError = false;

//std::map<const string, Texture_t *> Texture_t::Textures;

void detectGlfwError()
{
	const char *description;
	int code = glfwGetError(&description);
	if (description)
		printf("code %i description %s\n", code, description);
}

void detectOpenGLError(const std::string where)
{
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		printf("ogl err %i %s", err, where);
	}
}

void renderer_init()
{
	sceneDef = new Scene;

	personCam = new FirstPersonCamera;
	viewerCam = new ViewerCamera;

	sceneCur = sceneDef;

	cameraCur = personCam;

	sceneDef->bigGroup->Add(personCam->drawGroup);

	//renderTargetDef = new RenderTarget(width, height, GL_RGB, GL_UNSIGNED_BYTE);

	SetShaderSources();
}