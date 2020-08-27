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
	void view_nif(nif_t *);

	void programGo();
	void programLoop();

	extern int width, height;
	extern float delta;

	extern std::string OLDRIM;

	extern Mesh *viewed;
	
	extern bsa_t *interface;
	extern bsa_t *meshes;
	extern bsa_t *animations;
	extern bsa_t *textures;
	
} // namespace dark2

#endif