#include <libs>
#include <Gloom/Gloom.h>
#include <Gloom/Obj.h>

#include <algorithm>

#include <OpenGL/Types.h>
#include <OpenGL/Camera.h>
#include <OpenGL/Scene.h>
#include <OpenGL/Group.h>
#include <OpenGL/bound.h>
#include <OpenGL/Geometry.h>
#include <OpenGL/Material.h>

#include <Gloom/Files.h>

#include <Gloom/Interior.h>
#include <Gloom/Collision.h>
#include <Gloom/Mesh.h>
#include <Gloom/Actor.h>

namespace gloom
{
	unsigned int fps = 0;

	Interior *dungeon = nullptr;
	BodyPart *someDraugr = nullptr;
	BodyPart *meanSkelly = nullptr;
	Human *someHuman = nullptr;
	Player *player1 = nullptr;

	namespace MyKeys
	{
		bool w, a, s, d, r, f, v;
		bool shift, space;
	}

	const char *CURRENT_WRLD;
	const char *CURRENT_INTERIOR;

	std::string editme;

	FirstPersonCamera *first_person_camera;
	ViewerCamera *panCamera;
	RenderTarget *renderRarget;

	int width = 2560;
	int height = 1440;
	float delta = 0;
} // namespace gloom

void customLoad()
{
}

#include <list.hpp>

void load_plugins_archives()
{
	using namespace gloom;
	editme = fread("editme.txt");
	get_plugins()[0] = load_plugin(PLUGIN_ONE);
	get_plugins()[1] = load_plugin(PLUGIN_NAMESAKE);
	get_archives()[0] = load_archive(ARCHIVE_ONE);
	get_archives()[1] = load_archive(ARCHIVE_TWO);
	get_archives()[2] = load_archive(ARCHIVE_THREE);
	get_archives()[3] = load_archive(ARCHIVE_FOUR);
	get_archives()[4] = load_archive(ARCHIVE_FIVE);
	get_archives()[5] = load_archive(ARCHIVE_SIX);
}

int main()
{
	using namespace gloom;
	{
		using namespace MyKeys;
		w = a = s = d = r = f = v = false;
		shift = space = false;
	}
	CURRENT_WRLD = "Gloom";
	CURRENT_INTERIOR = "";
	load_plugins_archives();
	setup_glfw();
	first_person_camera = new FirstPersonCamera;
	panCamera = new ViewerCamera;
	opengl_init_scene();
	objs_init();
	setup_esc_menu();
	collision_init();
	camera_current = first_person_camera;
	Rc *rc = bsa_find_more("meshes\\clutter\\bucket02a.nif", 0x1);
	load_nif(rc, true);
	nifp_test();
#if 1
	// Secret bucket beginning
	simple_viewer(rc);
#endif
	simple_start_screen();
#if 0
	dungeon = new Interior("GloomGen"); // <-- interior to load
	dungeon->loadCell();
	someDraugr = new BodyPart("DraugrRace", "actors\\draugr\\character assets\\draugrmale.nif");
	someDraugr->PutDown("gloomgendraugr");
	meanSkelly = new BodyPart("DraugrRace", "actors\\draugr\\character assets\\draugrskeleton.nif");
	meanSkelly->PutDown("gloomgenskeleton");
	someHuman = new Human();
	someHuman->Place("gloomgenman");
#endif

	player1 = new Player();

	programLoop();
	return 1;
}
