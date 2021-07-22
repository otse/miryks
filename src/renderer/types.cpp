#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <renderer/types.h>

#include <renderer/camera.h>
#include <renderer/scene.h>
#include <renderer/shader.h>

#include <renderer/material.h>
#include <renderer/texture.h>

Camera *camera_current = nullptr;
Scene *scene_current = nullptr;
Scene *scene_default = nullptr;
RenderTarget *render_target_default = nullptr;

RenderSettings renderSettings;

bool checkGlError = false;

//std::map<const string, Texture_t *> Texture_t::Textures;

void detectGlfwError()
{
	const char *description;
	int code = glfwGetError(&description);
	if (description)
		printf("code ", code, "description ", description);
}

void detectOpenGLError(const std::string where)
{
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		printf("ogl err ", err, " after ", where);
	}
}

void opengl_init_scene()
{
	camera_current = new Camera;
	scene_default = new Scene;

	scene_current = scene_default;

	SetShaderSources();
}