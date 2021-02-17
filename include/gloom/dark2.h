#ifndef GLOOM_DARK2_H
#define GLOOM_DARK2_H

#include <opengl/types.h>

struct Grup;
struct Record;
struct Subrecord;
struct Rc;
struct Esp;
struct Bsa;

struct Nifp;
struct NifpRd;

typedef Esp Plugin;
typedef Bsa Archive;
typedef Nifp Nif;
typedef NifpRd Rd;

namespace gloom
{
};
namespace Glm = gloom;

namespace gloom
{
	class Obj;
	class Ref;
	class Object;
	class ObjectArray;
	class Interior;
	class Mesh;
	class SkinnedMesh;
	class Actor;
	class Human;
	class Player;
	class Skeleton;
	class Bone;
	class Keyframes;
	class Animation;
	class CSphere;
	class CShape;
	class Collider;

	Rc *LoadRc(const char *, const char *, unsigned long);
	Nif *LoadNif(Rc *, bool useCache = true);
	Plugin *LoadPlugin(const char *);
	Archive *LoadArchive(const char *);

	const auto newId = LoadRc;
	extern unsigned int fps;

	void programGo();
	void programLoop();

	void View(Rc *);
	void doImGui();
	void renderImGui();

	void HideCursor();
	void ShowCursor();

	extern int width, height;
	extern float delta;

	extern std::string pathToOldrim;

	// Todo, these globals just
	extern FirstPersonCamera *first_person_camera;
	extern ViewerCamera *viewer_camera;
	extern RenderTarget *renderRarget;

	extern Interior *dungeon;
	extern Actor *someDraugr;
	extern Actor *meanSkelly;
	extern Human *someHuman;
	extern Player *player1;

	// Nifp float structs to glm:

	inline vec2 *cast_vec_2(float *f) { return reinterpret_cast<vec2 *>(f); }
	inline vec3 *cast_vec_3(float *f) { return reinterpret_cast<vec3 *>(f); }
	inline vec4 *cast_vec_4(float *f) { return reinterpret_cast<vec4 *>(f); }
	inline mat3 *cast_mat_3(float *f) { return reinterpret_cast<mat3 *>(f); }
	inline mat4 *cast_mat_4(float *f) { return reinterpret_cast<mat4 *>(f); }

#define Gloom_Vec_3(X) *cast_vec_3((float *)&X)
#define Gloom_Vec_4(X) *cast_vec_4((float *)&X)
#define Gloom_Mat_3(X) *cast_mat_3((float *)&X)

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