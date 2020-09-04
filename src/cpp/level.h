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

class Level
{
public:
	Level();

	void Update();
};
