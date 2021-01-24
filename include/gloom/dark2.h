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
	class Object;
	class Level;
	class Mesh;
	class SkinnedMesh;
	class SkinPartition;
	class Actor;
	class Human;
	class Player;
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

	Rc *loadrc(const char *, const char *, unsigned long);
	Nifp *loadnifp(Rc *, int);

	Esp *LoadPlugin(const char *);
	Bsa *LoadArchive(const char *);

	void programGo();
	void programLoop();

	void doImGui();
	void renderImGui();

	void HideCursor();
	void ShowCursor();

	extern int width, height;
	extern float delta;

	extern std::string pathToOldrim;

	extern FirstPersonCamera *first_person_camera;
	extern ViewerCamera *viewer_camera;
	extern RenderTarget *render_target;

	extern Level *dungeon;
	extern Actor *someDraugr;
	extern Actor *meanSkelly;
	extern Human *someHuman;
	extern Player *player1;

	vec2 *cast_vec_2(float *f);
	vec3 *cast_vec_3(float *f);
	vec4 *cast_vec_4(float *f);
	mat3 *cast_mat_3(float *f);
	mat4 *cast_mat_4(float *f);

} // namespace gloom

#define READ_BSA_RESOURCE "Load"
#define VIEW_NIF "View"

#endif