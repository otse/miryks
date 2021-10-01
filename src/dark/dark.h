#pragma once

#include <lib.h>

#include <dark/list>

#include <skyrim/skyrim.h>

#include <renderer/renderer.h>

using namespace skyrim;

namespace dark
{
	class Ref;
	class Player;

	void darkassert(bool);

	extern Interior *dungeon;
	extern Player *player1;
	extern Monster *someDraugr, *meanSkelly;
	extern Char *someHuman;

	void reload_esp();
	void reload_dungeon();
	
	void goingrate();
	void program_while();

	void in_place_viewer(RES);

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