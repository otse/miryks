#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "types"

#include "camera"
#include "scene"
#include "shader"

#include "material"
#include "texture"

Shader_t *sha_basic;
Shader_t *sha_simple;
Shader_t *sha_fx;
Shader_t *sha_pbr;

Camera_t *camera;
Scene_t *scene;

bool checkGlError = false;

std::map<const string, Texture_t *> Texture_t::textures;

void detect_glfw_error()
{
	const char *description;
	int code = glfwGetError(&description);
	if (description)
		log_("code ", code, "description ", description);
}

void detect_opengl_error(const string where)
{
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		log_("ogl err ", err, " after ", where);
	}
}

Texture_t *get_texture(const string &path)
{
	auto has = Texture_t::textures.find(path);
	if (has != Texture_t::textures.end())
	{
		return has->second;
	}

	Texture_t *texture = new Texture_t(path);

	Texture_t::textures.emplace(path, texture);

	return texture;
}

void opengl_reload_shaders()
{
	log_("reload simple / fx / pbr");

	sha_basic->Reload();
	sha_simple->Reload();
	sha_fx->Reload();
	sha_pbr->Reload();
}

void opengl_go()
{
	log_("opengl go");

	camera = new Camera_t;
	scene = new Scene_t;

	sha_basic = new Shader_t;
	sha_simple = new Shader_t;
	sha_fx = new Shader_t;
	sha_pbr = new Shader_t;

	sha_basic->Load("basic.vert", "basic.frag");
	sha_simple->Load("simple.vert", "simple.frag");
	sha_fx->Load("fx.vert", "fx.frag");
	sha_pbr->Load("pbr.vert", "pbr.frag");
}