#ifndef DARK2_H
#define DARK2_H

typedef struct nif Nif;
typedef struct bsa Bsa;
typedef struct esp Esp;
typedef struct record Record;
typedef struct grup Grup;

typedef struct rc Rc;
struct esp_t;

#define ONE_SKYRIM_UNIT_IN_CM 1.428

#include "opengl/types"

#include "idiom.hpp"

namespace dark2
{
	vec2 *cast_vec_2(float *f);
	vec3 *cast_vec_3(float *f);
	vec4 *cast_vec_4(float *f);
	mat3 *cast_mat_3(float *f);
	mat4 *cast_mat_4(float *f);
	
	class Level;
	class Mesh;

	namespace viewer
	{
		extern Mesh *viewed_mesh;
		extern Renderable *viewed_object;

		void spotlight(Rc *);
	} // namespace viewer

	nif *LoadNif(rc *);
	esp *LoadPlugin(const char *);
	bsa *LoadArchive(const char *);

	void programGo();
	void programLoop();

	extern int width, height;
	extern float delta;

	extern std::string OLDRIM;

	extern Mesh *viewed_mesh;
	extern Renderable *viewed_object;

	extern FirstPersonCamera *first_person_camera;
	extern ViewerCamera *viewer_camera;

	extern Level *dungeon;

	extern esp *skyrim;
	extern esp *padstow;

	extern bsa *interface;
	extern bsa *meshes;
	extern bsa *animations;
	extern bsa *textures;
	extern bsa *hirestexturepack01;
	extern bsa *hirestexturepack02;
	extern bsa *hirestexturepack03;

} // namespace dark2

#define READ_BSA_RESOURCE "Load"
#define VIEW_NIF "View"

#endif