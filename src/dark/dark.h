#pragma once

#include <lib.h>

#include <dark/list>

#include <skyrim/skyrim.h>

#include <renderer/renderer.h>

using namespace skyrim;

void overlay_plugins();
void overlay_archives();
void overlay_models();
void overlay_cellview();
void overlay_rstats(bool *);

namespace dark
{
	class Reference;
	class Player;

	extern Player *player1;
	extern Monster *someDraugr, *meanSkelly;
	extern Char *someHuman;

	extern std::map<const char *, int> keys;

	void now_you_see_me();
	void now_you_dont();
	bool pressing_key(const char *);
	bool holding_key(const char *);

	void darkassert(bool);

	void load_bucket();
	void load_gloomgen();
	void load_darkworld();

	void view_in_place(Res *);

	void init();
	void loop();

	void reload_my_esp();
	void reload_dungeon();
	void unset_dungeon();
	void third_person();

	inline vec2 cast_vec2(void *f) { return *reinterpret_cast<vec2 *>(f); }
	inline vec3 cast_vec3(void *f) { return *reinterpret_cast<vec3 *>(f); }
	inline vec4 cast_vec4(void *f) { return *reinterpret_cast<vec4 *>(f); }
	inline bvec4 cast_bvec4(void *u) { return *reinterpret_cast<bvec4 *>(u); }
	inline mat3 cast_mat3(void *f) { return *reinterpret_cast<mat3 *>(f); }
	inline mat4 cast_mat4(void *f) { return *reinterpret_cast<mat4 *>(f); }
}

void hero_menu();

void get_img();