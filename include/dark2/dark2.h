#ifndef DARK2_H
#define DARK2_H

#define ONE_SKYRIM_UNIT_IN_CM 1.428

#define EYE_HEIGHT 160 / ONE_SKYRIM_UNIT_IN_CM

#include <opengl/types.h>

struct grup;
struct record;
struct subrecord;
struct rc;
struct nifp;
struct esp;
struct bsa;

namespace dark2
{
	class Level;
	class Mesh;

	namespace viewer
	{
		void view(rc *);
		extern Mesh *viewed_mesh;
		extern Renderable *viewed_object;
	} // namespace viewer

	extern unsigned int fps;

	nifp *loadNif(rc *);
	esp *loadPlugin(const char *);
	bsa *loadArchive(const char *);

	void programGo();
	void programLoop();

	void doImGui();
	void renderImGui();

	void HideCursor();
	void ShowCursor();

	extern int width, height;
	extern float delta;

	extern std::string pathToOldrim;

	extern Mesh *viewed_mesh;
	extern Renderable *viewed_object;

	extern FirstPersonCamera *first_person_camera;
	extern ViewerCamera *viewer_camera;
	extern RenderTarget *render_target;

	extern Level *dungeon;

	vec2 *cast_vec_2(float *f);
	vec3 *cast_vec_3(float *f);
	vec4 *cast_vec_4(float *f);
	mat3 *cast_mat_3(float *f);
	mat4 *cast_mat_4(float *f);

} // namespace dark2

#define READ_BSA_RESOURCE "Load"
#define VIEW_NIF "View"

#endif