#ifndef DARK2_H
#define DARK2_H

struct bsa_t;

#include "opengl/types"

#include "idiom.hpp"

namespace dark2
{
	void program_go();
	void program_loop();

	extern int width, height;
	extern float delta;

	extern string OLDRIM_PATH;
	
	extern bsa_t interface;
	extern bsa_t meshes;
	extern bsa_t animations;

	
} // namespace dark2

#endif