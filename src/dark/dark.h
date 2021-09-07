#pragma once

#include <lib.h>

#include <renderer/renderer.h>

#include <dark/list>

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

namespace skyrim
{
	class Mesh;
	class SkinnedMesh;
	class Skeleton;
	class Keyframes;
	class Animation;
	class Interior;
}
using namespace skyrim;

namespace dark
{
	void darkassert(bool);

	extern char *editme;

	extern std::map<void *, Nif *> nifs;

	int ext_nif_save(void *, Nif *);
	Nif *ext_nif_saved(void *);

	Rsc *load_rsc(const char *, const char * = "meshes\\", unsigned long = 0x1);
	Nif *load_model(Rsc *, bool);
	Esp *load_plugin(const char *, bool = true);
	Bsa *load_archive(const char *);
	void load_these_definitions(espp plugin);
	
	Keyframes *load_keyframes_from_disk(const char *);
	Mesh *create_simple_mesh_from_modl(const char *, bool);

	void reload_my_plugin();
	void reload_dungeon();
	
	void goingrate();
	void program_while();

	void simple_viewer(Nif *);
	void doImGui();
	void renderImGui();

	void hide_cursor();
	void show_cursor();

	extern std::map<const char *, int> keys;

	inline bool pressing(const char *id) {
		return keys[id] == 1;
	}
	inline bool holding(const char *id) {
		return keys[id] >= 1;
	}

	extern Interior *dungeon;
	extern Creature *someDraugr, *meanSkelly;
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

void get_img();

void put_it_fullscreen();

void setup_esc_menu();
void esc_menu(bool *);