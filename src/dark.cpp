#include <png.h>

#include <core/files.h>
#include <core/basefile.h>

#include <dark/dark.h>

#include <skyrim/interior.h>

using namespace dark;
using namespace skyrim;

void load_bucket()
{
	Rc *rc = bsa_find_more("meshes\\clutter\\bucket02a.nif", 0x1);
	Nif *nif = import_nif(rc, true);
	simple_viewer(nif);
}

void load_gloomgen()
{
	dungeon = new Interior("GloomGen");
	dungeon->load();
	//player1 = new Player();
}

void load_plugins_archives()
{
	editme = fread("editme.txt");
	get_plugins()[0] = load_plugin(PLUGIN_0);
	get_plugins()[1] = load_plugin(PLUGIN_1);
	get_plugins()[2] = load_plugin(PLUGIN_2);
	get_plugins()[3] = load_plugin(PLUGIN_3);
	get_plugins()[4] = load_plugin(PLUGIN_4);
	get_plugins()[5] = load_plugin(PLUGIN_5);
	get_archives()[0] = load_archive(ARCHIVE_0);
	//get_archives()[1] = load_archive(ARCHIVE_1);
	//get_archives()[2] = load_archive(ARCHIVE_2);
	get_archives()[3] = load_archive(ARCHIVE_3);
	get_archives()[4] = load_archive(ARCHIVE_4);
	get_archives()[5] = load_archive(ARCHIVE_5);
	//get_archives()[6] = load_archive(ARCHIVE_6);
	//get_archives()[7] = load_archive(ARCHIVE_7);
	get_archives()[8] = load_archive(ARCHIVE_8);
	get_archives()[9] = load_archive(ARCHIVE_9);
	get_archives()[10] = load_archive(ARCHIVE_10);
	get_archives()[11] = load_archive(ARCHIVE_11);
	get_archives()[12] = load_archive(ARCHIVE_12);
	get_archives()[13] = load_archive(ARCHIVE_13);
	get_archives()[14] = load_archive(ARCHIVE_14);
	get_archives()[15] = load_archive(ARCHIVE_15);
	get_archives()[16] = load_archive(ARCHIVE_16);
	get_archives()[17] = load_archive(ARCHIVE_17);
}

#include <dark/collision.h>

#include <renderer/camera.h>
#include <renderer/rendertarget.h>

int main()
{
	{
		using namespace MyKeys;
		w = a = s = d = r = f = v = false;
		shift = space = false;
	}
	basefile_handle("basefile");
	setup_glfw();
	load_plugins_archives();
	nif_test();
	opengl_init_scene();
	printf("now collide\n");
	collision_init();
	cameraCur = personCam;
#if 1
	// Bucket beginning
	load_bucket();
#endif
	put_it_fullscreen();
#if 1
	load_gloomgen();
	//someDraugr = new BodyPart("DraugrRace", "actors\\draugr\\character assets\\draugrmale.nif");
	//someDraugr->PutDown("gloomgendraugr");
	//meanSkelly = new BodyPart("DraugrRace", "actors\\draugr\\character assets\\draugrskeleton.nif");
	//meanSkelly->PutDown("gloomgenskeleton");
	//someHuman = new Human();
	//someHuman->Place("gloomgenman");
#endif
\
	//player1 = new Player();
	program_while();
	return 1;
}

namespace dark
{
	void reload_my_plugin()
	{
		esppp plugin = &get_plugins()[MY_ESP];
		free_plugin(plugin);
		*plugin = load_plugin(PLUGIN_5, true);
	}

	void reload_dungeon()
	{
		if (dungeon)
		{
			const char *edId = dungeon->edId;
			delete dungeon;
			dungeon = new Interior(edId);
			dungeon->alreadyTeleported = true;
			dungeon->load();
		}
	}
}

#include <skyrim/mesh.h>

#include <renderer/scene.h>
#include <renderer/group.h>
#include <renderer/drawgroup.h>

// todo uglies
void dark::simple_viewer(Nif *nif)
{
	static Mesh *mesh = nullptr;
	static DrawGroup *drawGroup = nullptr;
	if (mesh)
	{
		sceneDef->drawGroups.Remove(drawGroup);
		delete mesh;
		delete drawGroup;
	}
	mesh = new Mesh(nif);
	drawGroup = new DrawGroup(
		mesh->baseGroup, translate(mat4(1.0), personCam->pos));
	sceneDef->drawGroups.Add(drawGroup);
	HideCursor();
	cameraCur = viewerCam;
	viewerCam->pos = drawGroup->aabb.center();
	//viewerCam->pos = personCam->pos;
	viewerCam->radius = drawGroup->aabb.radius2() * 2;
}