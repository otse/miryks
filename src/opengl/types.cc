#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "types"

#include "camera"
#include "scene"
#include "shader"

#include "material"
#include "texture"

shader_t *sha_basic;
shader_t *sha_simple;
shader_t *sha_fx;
shader_t *sha_pbr;

camera_t *camera;
scene_t *scene;

bool checkGlError = false;

std::map<const string, texture_t *> texture_t::textures;

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

texture_t *get_texture(const string &path)
{
	auto has = texture_t::textures.find(path);
	if (has != texture_t::textures.end())
	{
		return has->second;
	}

	texture_t *texture = new texture_t(path);

	texture_t::textures.emplace(path, texture);

	return texture;
}

void opengl_reload_shaders()
{
	log_("reload simple / fx / pbr");

	sha_basic->reload();
	sha_simple->reload();
	sha_fx->reload();
	sha_pbr->reload();
}

void opengl_go()
{
	log_("opengl go");

	camera = new camera_t;
	scene = new scene_t;

	sha_basic = new shader_t;
	sha_simple = new shader_t;
	sha_fx = new shader_t;
	sha_pbr = new shader_t;

	//sha_basic->load("basic.vert", "basic.frag");
	//sha_simple->load("simple.vert", "simple.frag");
	//sha_fx->load("fx.vert", "fx.frag");
	//sha_pbr->load("pbr.vert", "pbr.frag");
}