#include <libs>
#include <resourcefile.h>

#include <png.h>

#include <dark/dark.h>
#include <dark/Obj.h>

#include <Gooey/Yagrum.h>

#include <algorithm>

#include <opengl/Types.h>
#include <opengl/Camera.h>
#include <opengl/Scene.h>
#include <opengl/Group.h>
#include <opengl/Geometry.h>
#include <opengl/Material.h>

#include <dark/Files.h>

#include <dark/Interior.h>
#include <dark/Collision.h>
#include <dark/mesh.h>
#include <dark/Actor.h>

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

	const char *CURRENT_WRLD;
	const char *CURRENT_INTERIOR;

	std::string editme;

	FirstPersonCamera *first_person_camera;
	ViewerCamera *pan_camera;
	RenderTarget *render_target;

	int width = 2560;
	int height = 1440;
	float delta = 0;
}