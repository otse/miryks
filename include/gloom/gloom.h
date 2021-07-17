#ifndef GLOOM_DARK2_H
#define GLOOM_DARK2_H

// part of gloom

#include <OpenGL/Types.h>

#include "defs"

namespace gloom
{
	Rc *load_rc(const char *, const char *, unsigned long);
	Nif *load_nif(Rc *, bool);
	Mesh *load_mesh(const char *, bool);
	Esp *load_plugin(const char *, bool = true);
	Bsa *load_archive(const char *);

	extern unsigned int fps;

	void programGo();
	void programLoop();

	void View(Resource *);
	void doImGui();
	void renderImGui();

	void HideCursor();
	void ShowCursor();

	extern int width, height;
	extern float delta;

	extern std::string editme;

	// Todo, these globals just
	extern FirstPersonCamera *firstPersonCamera;
	extern ViewerCamera *panCamera;
	extern RenderTarget *renderRarget;

	namespace MyKeys {
		extern bool w, a, s, d, r, f, v;
		extern bool shift, space;
	};
	
	extern Interior *dungeon;
	extern BodyPart *someDraugr;
	extern BodyPart *meanSkelly;
	extern Human *someHuman;
	extern Player *player1;

	// Nifp float structs to glm:

	inline vec2 *cast_vec_2(float *f) { return reinterpret_cast<vec2 *>(f); }
	inline vec3 *cast_vec_3(float *f) { return reinterpret_cast<vec3 *>(f); }
	inline vec4 *cast_vec_4(float *f) { return reinterpret_cast<vec4 *>(f); }
	inline mat3 *cast_mat_3(float *f) { return reinterpret_cast<mat3 *>(f); }
	inline mat4 *cast_mat_4(float *f) { return reinterpret_cast<mat4 *>(f); }

#define gloomVec3(X) *cast_vec_3((float *)&X)
#define gloomVec4(X) *cast_vec_4((float *)&X)
#define gloomMat3(X) *cast_mat_3((float *)&X)

} // namespace gloom

void esp_gui();
void bsa_gui();
void nifp_gui();
void cell_gui();
void hero_menu();
void render_stats(bool *);

void setup_esc_menu();
void esc_menu(bool *);

#define READ_BSA_RESOURCE "Load"
#define VIEW_NIF "View"

#endif