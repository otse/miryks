#include <png.h>

#include <core/files.h>
#include <core/basefile.h>

#include <lib.h>

#include <dark/dark.h>

#include <skyrim/interior.h>

using namespace dark;
using namespace skyrim;

void load_bucket()
{
	return;
	Rc *rc = bsa_find_more("meshes\\clutter\\bucket02a.nif", 0x1);
	import_nif(rc, true);
	simple_viewer(rc);
}

void load_gloomgen()
{
	dungeon = new Interior("GloomGen");
	dungeon->loadcell();
	//player1 = new Player();
}

void load_plugins_archives()
{
	editme = fread("editme.txt");
	get_plugins()[0] = load_plugin(PLUGIN_ONE);
	get_plugins()[1] = load_plugin(PLUGIN_TWO);
	get_plugins()[2] = load_plugin(PLUGIN_THREE);
	get_plugins()[3] = load_plugin(PLUGIN_NAMESAKE);
	get_archives()[0] = load_archive(ARCHIVE_ONE);
	get_archives()[1] = load_archive(ARCHIVE_TWO);
	get_archives()[2] = load_archive(ARCHIVE_THREE);
	get_archives()[3] = load_archive(ARCHIVE_FOUR);
	get_archives()[4] = load_archive(ARCHIVE_FIVE);
	get_archives()[6] = load_archive(ARCHIVE_SIX);
	get_archives()[7] = load_archive(ARCHIVE_SEVEN);
	get_archives()[8] = load_archive(ARCHIVE_EIGHT);
}

#include <dark/collision.h>

#include <renderer/camera.h>
#include <renderer/rendertarget.h>

#include <gooey/yagrum.h>

int main()
{
	{
		using namespace MyKeys;
		w = a = s = d = r = f = v = false;
		shift = space = false;
	}
	CURRENT_INTERIOR = "";
	basefile_handle("basefile");
	setup_glfw();
	load_yagrum();
	load_plugins_archives();
	first_person_camera = new FirstPersonCamera;
	pan_camera = new ViewerCamera;
	opengl_init_scene();
	render_target_default = new RenderTarget(width, height, GL_RGB, GL_UNSIGNED_BYTE);
	collision_init();
	camera_current = first_person_camera;
	//Rc *rc = bsa_find_more("meshes\\clutter\\bucket02a.nif", 0x1);
	//import_nif(rc, true);
	//nifp_test();
#if 0
	// Secret bucket beginning
	simple_viewer(rc);
#endif
	put_it_fullscreen();
	yagrum_queue("", 2.0, true);
	yagrum_set_rotate_speed(3.0);
	// simple_start_screen();
#if 1
	load_gloomgen();
	//someDraugr = new BodyPart("DraugrRace", "actors\\draugr\\character assets\\draugrmale.nif");
	//someDraugr->PutDown("gloomgendraugr");
	//meanSkelly = new BodyPart("DraugrRace", "actors\\draugr\\character assets\\draugrskeleton.nif");
	//meanSkelly->PutDown("gloomgenskeleton");
	//someHuman = new Human();
	//someHuman->Place("gloomgenman");
#endif

	//player1 = new Player();

	programLoop();
	return 1;
}

#include <skyrim/mesh.h>

#include <renderer/scene.h>
#include <renderer/group.h>
#include <renderer/drawgroup.h>

void dark::simple_viewer(Rc *rc)
{
	static Mesh *mesh = nullptr;
	static DrawGroup *drawGroup = nullptr;
	if (mesh)
	{
		scene_default->drawGroups.Remove(drawGroup);
		delete mesh;
		delete drawGroup;
	}
	Nif *nif = import_nif(rc, false);
	nifp_save(rc, nif);
	// create_mesh()
	mesh = new Mesh(nif);
	drawGroup = new DrawGroup(mesh->baseGroup, translate(mat4(1.0), first_person_camera->pos));
	scene_default->drawGroups.Add(drawGroup);
	HideCursor();
	camera_current = pan_camera;
	pan_camera->pos = drawGroup->aabb.center();
	//pan_camera->pos = first_person_camera->pos;
	pan_camera->radius = drawGroup->aabb.radius2() * 2;
}