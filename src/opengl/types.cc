#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "types"

#include "camera"
#include "scene"
#include "shader"

#include "material"
#include "texture"

Shader_t *shaBasic;
Shader_t *shaSimple;
Shader_t *shaFx;
Shader_t *shaPbr;

Camera_t *camera;
Scene_t *scene;

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

void oglReloadShaders()
{
	log_("reload simple / fx / pbr");

	shaBasic->Reload();
	shaSimple->Reload();
	shaFx->Reload();
	shaPbr->Reload();
}

void oglGo()
{
	log_("opengl go");

	camera = new Camera_t;
	scene = new Scene_t;

	shaBasic = new Shader_t;
	shaSimple = new Shader_t;
	shaFx = new Shader_t;
	shaPbr = new Shader_t;

	shaBasic->Load("basic.vert", "basic.frag");
	shaSimple->Load("simple.vert", "simple.frag");
	shaFx->Load("fx.vert", "fx.frag");
	shaPbr->Load("pbr.vert", "pbr.frag");
}