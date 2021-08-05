#include <lib.h>
#include <core/basefile.h>

#include <png.h>

#include <dark/dark.h>

#include <gooey/yagrum.h>

#include <algorithm>

#include <renderer/types.h>
#include <renderer/camera.h>
#include <renderer/scene.h>
#include <renderer/group.h>
#include <renderer/geometry.h>
#include <renderer/material.h>

#include <core/files.h>

#include <skyrim/interior.h>
#include <skyrim/mesh.h>

#include <dark/collision.h>

#include <dark/actor.h>

namespace dark
{
	unsigned int fps = 0;

	Interior *dungeon = nullptr;
	BodyPart *someDraugr = nullptr;
	BodyPart *meanSkelly = nullptr;
	Human *someHuman = nullptr;
	Player *player1 = nullptr;

	Image *yagrum_image = nullptr;

	namespace MyKeys
	{
		bool w, a, s, d, r, f, v;
		bool shift, space;
	}

	const char *CURRENT_INTERIOR;

	std::string editme;

	RenderTarget *render_target;

	int width = 2560;
	int height = 1440;
	float delta = 0;
}