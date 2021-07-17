#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <OpenGL/Types.h>

#include <OpenGL/Camera.h>
#include <OpenGL/Scene.h>
#include <OpenGL/Shader.h>

#include <OpenGL/Material.h>
#include <OpenGL/Texture.h>

Camera *cameraCurrent = nullptr;
Scene *sceneCurrent = nullptr;
Scene *sceneDefault = nullptr;

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
	cameraCurrent = new Camera;
	sceneDefault = new Scene;

	sceneCurrent = sceneDefault;

	SetShaderSources();
}