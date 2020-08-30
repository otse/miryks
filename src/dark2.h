#ifndef DARK2_H
#define DARK2_H

struct Bsa;
struct Esp;
struct Rc;
struct Nif;
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

	void spotlight(Rc *);
	}

	//namespace importers
	//{
	Nif *make_nif(Rc *);
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
	
	extern Esp *skyrim;

	extern Bsa *interface;
	extern Bsa *meshes;
	extern Bsa *animations;
	extern Bsa *textures;
	
} // namespace dark2

#endif