#pragma once

#include <lib.h>

#include <renderer/types.h>

#include "list"

namespace dark
{
	class Ref;
	class Actor;
	class BodyPart;
	class Human;
	class Player;
	
	class CSphere;
	class CShape;
	class Collider;

	class Image;
}

namespace skyrim
{
	class Mesh;
	class Interior;
}
using namespace skyrim;

namespace dark
{
	extern std::map<void *, Nif *> nifs;

	int ext_nif_save(void *, Nif *);
	Nif *ext_nif_saved(void *);
	Rc *load_rc(const char *, const char *, unsigned long);
	Nif *import_nif(Rc *, bool);
	Mesh *create_simple_mesh_from_modl(const char *, bool);
	Esp *load_plugin(const char *, bool = true);
	Bsa *load_archive(const char *);

	extern unsigned int fps;

	void reload_my_plugin();
	void reload_dungeon();
	
	void load_definitions(espp plugin);
	void setup_glfw();
	void program_while();

	void simple_viewer(Nif *);
	void doImGui();
	void renderImGui();

	void HideCursor();
	void ShowCursor();

	extern std::string editme;

	extern RenderTarget *render_target;

	namespace MyKeys
	{
		extern bool w, a, s, d, r, f, v;
		extern bool shift, space;
	};

	extern Interior *dungeon;
	extern BodyPart *someDraugr;
	extern BodyPart *meanSkelly;
	extern Human *someHuman;
	extern Player *player1;

	inline vec2 cast_vec2(void *f) { return *reinterpret_cast<vec2 *>(f); }
	inline vec3 cast_vec3(void *f) { return *reinterpret_cast<vec3 *>(f); }
	inline vec4 cast_vec4(void *f) { return *reinterpret_cast<vec4 *>(f); }
	inline bvec4 cast_bvec4(void *u) { return *reinterpret_cast<bvec4 *>(u); }
	inline mat3 cast_mat3(void *f) { return *reinterpret_cast<mat3 *>(f); }
	inline mat4 cast_mat4(void *f) { return *reinterpret_cast<mat4 *>(f); }

}

void esp_gui();
void bsa_gui();
void nif_gui();
void cell_gui();
void hero_menu();
void render_stats(bool *);

void simple_loader();
void load_bucket();
void load_gloomgen();
void get_img();

void put_it_fullscreen();

void setup_esc_menu();
void esc_menu(bool *);

void make_timer();

#define READ_BSA_RESOURCE "Load"
#define VIEW_NIF "View"