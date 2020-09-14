#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <opengl/types.h>

#include <opengl/camera.h>
#include <opengl/scene.h>
#include <opengl/shader.h>

#include <opengl/material.h>
#include <opengl/texture.h>

Camera *camera;
Scene *scene;

RenderSettings renderSettings;

bool checkGlError = false;

//std::map<const string, Texture_t *> Texture_t::Textures;

void detectGlfwError()
{
	const char *description;
	int code = glfwGetError(&description);
	if (description)
		log_("code ", code, "description ", description);
}

void detectOpenGLError(const string where)
{
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		log_("ogl err ", err, " after ", where);
	}
}

void OGLGo()
{
	log_("OGL go");

	camera = new Camera;
	scene = new Scene;

	SetShaderSources();
}