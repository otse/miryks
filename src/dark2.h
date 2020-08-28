#ifndef DARK2_H
#define DARK2_H

struct bsa_t;
struct rc_t;
struct nif_t;
class Mesh;

#include "cpp/opengl/types"

#include "idiom.hpp"

namespace dark2
{
	nif_t *nif_from_rc(rc_t *);
	void nif_viewer(nif_t *);

	void programGo();
	void programLoop();

	extern int width, height;
	extern float delta;

	extern std::string OLDRIM;

	extern Mesh *viewed_mesh;
	extern Renderable *viewed_object;

	extern FirstPersonCamera *first_person_camera;
	extern ViewerCamera *viewer_camera;
	
	extern bsa_t *interface;
	extern bsa_t *meshes;
	extern bsa_t *animations;
	extern bsa_t *textures;
	
} // namespace dark2

#endif