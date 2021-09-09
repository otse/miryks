#pragma once

#include <lib.h>

#include <dark/list>

#include <skyrim/skyrim.h>

#include <renderer/renderer.h>

using namespace skyrim;

namespace dark
{
	class Ref;
	class Actor;
	class Creature;
	class Player;
	class BodyPart;
	class Human;
	class Image;
}

namespace dark
{
	void darkassert(bool);

	extern char *editme;

	int ext_nif_save(const char *, Nif *);
	Nif *ext_nif_saved(const char *);
	std::map<const char *, Nif *> &ext_nif_map();

	Rsc *load_res(const char *, const char * = "meshes\\", unsigned long = 0x1);
	Nif *load_model(Rsc *);
	Esp *load_plugin(const char *, bool = true);
	Bsa *load_archive(const char *);
	void load_these_definitions(Esp *);
	
	Keyframes *load_keyframes_from_disk(const char *);

	extern Interior *dungeon;
	extern Player *player1;
	extern Creature *someDraugr, *meanSkelly;
	extern Human *someHuman;

	void reload_esp();
	void reload_dungeon();
	
	void goingrate();
	void program_while();

	void simple_viewer(Nif *);

	void hide_cursor();
	void show_cursor();

	extern std::map<const char *, int> keys;

	inline bool pressing(const char *id) {
		return keys[id] == 1;
	}
	inline bool holding(const char *id) {
		return keys[id] >= 1;
	}
	
	inline vec2 cast_vec2(void *f) { return *reinterpret_cast<vec2 *>(f); }
	inline vec3 cast_vec3(void *f) { return *reinterpret_cast<vec3 *>(f); }
	inline vec4 cast_vec4(void *f) { return *reinterpret_cast<vec4 *>(f); }
	inline bvec4 cast_bvec4(void *u) { return *reinterpret_cast<bvec4 *>(u); }
	inline mat3 cast_mat3(void *f) { return *reinterpret_cast<mat3 *>(f); }
	inline mat4 cast_mat4(void *f) { return *reinterpret_cast<mat4 *>(f); }
}

void overlay_plugins();
void overlay_archives();
void overlay_models();

void cell_gui();
void hero_menu();
void render_stats(bool *);

void get_img();

void put_it_fullscreen();