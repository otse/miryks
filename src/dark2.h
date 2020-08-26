#ifndef DARK2_H
#define DARK2_H

struct bsa_t;

#include "cpp/opengl/types"

#include "idiom.hpp"

namespace dark2
{
	void programGo();
	void programLoop();

	extern int width, height;
	extern float delta;

	extern std::string OLDRIM;
	
	extern bsa_t *interface;
	extern bsa_t *meshes;
	extern bsa_t *animations;
	extern bsa_t *textures;
	
} // namespace dark2

#endif