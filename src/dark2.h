#ifndef DARK2_H
#define DARK2_H

struct bsa_t;
struct rc_t;
struct nif_t;
class Mesh;

#define ONE_SKYRIM_UNIT_IN_CM 1.428

#include "cpp/opengl/types"

#include "idiom.hpp"

namespace dark2
{
	namespace viewer
	{
	extern Mesh *viewed_mesh;
	extern Renderable *viewed_object;

	void spotlight(rc_t *);
	}

	//namespace importers
	//{
	nif_t *make_nif(rc_t *);
	//}
	void programGo();
	void programLoop();

	extern int width, height;
	extern float delta;

	extern std::string OLDRIM;

	extern Mesh *viewed_mesh;
	extern Renderable *viewed_object;

	extern FirstPersonCamera *first_person_camera;
	extern ViewerCamera *viewer_camera;
	
	extern esp_t *skyrim;
	
	extern bsa_t *interface;
	extern bsa_t *meshes;
	extern bsa_t *animations;
	extern bsa_t *textures;
	
} // namespace dark2

#endif