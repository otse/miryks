#ifndef DARK2_H
#define DARK2_H

typedef struct nif Nif;
typedef struct bsa Bsa;
typedef struct esp Esp;
typedef struct record Record;
typedef struct grup Grup;

typedef struct rc Rc;
struct esp_t;

#define EYE_HEIGHT 175

#define ONE_SKYRIM_UNIT_IN_CM 1.428

#include <opengl/types.h>

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

	nif *loadNif(rc *);
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