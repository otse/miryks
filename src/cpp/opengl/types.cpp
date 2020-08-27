#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "types"

#include "camera"
#include "scene"
#include "shader"

#include "material"
#include "texture"

Shader *shaBasic;
Shader *shaSimple;
Shader *shaFx;
Shader *shaPbr;

Camera *camera;
Scene *scene;

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

	camera = new FirstPersonCamera;
	scene = new Scene;

	shaBasic = new Shader;
	shaSimple = new Shader;
	shaFx = new Shader;
	shaPbr = new Shader;

	shaBasic->Load("basic.vert", "basic.frag");
	shaSimple->Load("simple.vert", "simple.frag");
	shaFx->Load("fx.vert", "fx.frag");
	shaPbr->Load("pbr.vert", "pbr.frag");
}