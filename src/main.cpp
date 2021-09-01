
#include <dark/files.h>

#include <png.h>

#include <dark/dark.h>

#include <algorithm>

#include <renderer/types.h>
#include <renderer/camera.h>
#include <renderer/scene.h>
#include <renderer/group.h>
#include <renderer/geometry.h>
#include <renderer/material.h>


#include <skyrim/interior.h>
#include <skyrim/mesh.h>

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

	std::string editme;

	FirstPersonCamera *personCam;
	ViewerCamera *viewerCam;
	RenderTarget *render_target;

	// todo extern this
	int width = 1920;
	int height = 1080;
	float delta = 0;
}