#pragma once

#include "dark2.h"

extern "C"
{
#include "c/bsa.h"
#include "c/nif.h"
#include "c/esp.h"
}

#include "cpp/opengl/types"
#include "cpp/opengl/scene"
#include "cpp/opengl/group"
#include "cpp/opengl/geometry"
#include "cpp/opengl/material"

namespace dark2
{
struct Cell
{
	Record *cell;
	Grup *persistent;
	Grup *non_persistent;
};

class Level
{
public:
	Level();

	static Cell GetCell(const char *);

	void Update();
};
}