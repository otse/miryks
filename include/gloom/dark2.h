#ifndef GLOOM_DARK2_H
#define GLOOM_DARK2_H

#define ONE_SKYRIM_UNIT_IN_CM 1.428

#define EYE_HEIGHT 160 / ONE_SKYRIM_UNIT_IN_CM

#include <opengl/types.h>

struct Grup;
struct Record;
struct Subrecord;
struct Rc;

struct Nifp;
struct Esp;
struct Bsa;

namespace gloom
{
	class Obj;
	class Ref;
	class Object;
	class ObjectArray;
	class Interior;
	class Mesh;
	class SkinnedMesh;
	class SkinPartition;
	class Actor;
	class Human;
	class Player;
	class CSphere;
	class CShape;
	class Collider;
	class Bone;
	class Skeleton;
	class KeyFrames;
	class Animation;

	namespace viewer
	{
		void view(Rc *);
		extern Mesh *mesh;
		extern DrawGroup *drawGroup;
	} // namespace viewer

	extern unsigned int fps;

	// -- loaders.cxx
	Rc *loadRc(const char *, const char *, unsigned long);
	Nifp *loadNifp(Rc *, int);
	Esp *loadEsp(const char *);
	Bsa *loadBsa(const char *);

	void programGo();
	void programLoop();

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

	vec2 *cast_vec_2(float *);
	vec3 *cast_vec_3(float *);
	vec4 *cast_vec_4(float *);
	mat3 *cast_mat_3(float *);
	mat4 *cast_mat_4(float *);
	#define Gloom_Vec_3(X) *cast_vec_3( (float *) & X )
	#define Gloom_Vec_4(X) *cast_vec_4( (float *) & X )
	#define Gloom_Mat_3(X) *cast_mat_3( (float *) & X )

} // namespace gloom

void esp_gui();
void bsa_gui();
void nifp_gui();
void cell_gui();
void hero_menu();
void render_stats(bool *);

#define READ_BSA_RESOURCE "Load"
#define VIEW_NIF "View"

#endif