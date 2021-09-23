#include <png.h>

#include <dark/dark.h>
#include <dark/files.h>

#include <skyrim/interior.h>
#include <skyrim/actors.h>

#include <gooey/yagrum.h>

using namespace dark;
using namespace skyrim;

namespace dark
{
	Creature *someDraugr = nullptr, *meanSkelly = nullptr;
	Character *someHuman = nullptr;
}

namespace dark
{
//Player *player1 = nullptr;
Interior *dungeon = nullptr;
std::map<const char *, int> keys;
}

void darkassert(bool e)
{
	assertc(e);
}

void load_bucket()
{
	simple_viewer("clutter\\bucket02a.nif");
	yagrum_queue("", 10, true);
}

void load_gloomgen()
{
	dungeon = new Interior("GloomGen");
	dungeon->Load();
	//player1 = new Player();
}

void load_plugins_archives()
{
	get_plugins()[0] = load_plugin(PLUGIN_0);
	get_plugins()[1] = load_plugin(PLUGIN_1);
	get_plugins()[2] = load_plugin(PLUGIN_2);
	get_plugins()[3] = load_plugin(PLUGIN_3);
	get_plugins()[4] = load_plugin(PLUGIN_4);
	get_plugins()[5] = load_plugin(PLUGIN_5, true);
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

#include <renderer/camera.h>
#include <renderer/rendertarget.h>

int main()
{
	char **buf = &editme;
	fbuf("editme.txt", buf, true);
	goingrate();
	load_yagrum();
	load_plugins_archives();
	nif_test();
	renderer_init();
	load_bucket();
	refs_init();
	put_it_fullscreen();
	load_gloomgen();
	someDraugr = new Creature("DraugrRace", "actors\\draugr\\character assets\\draugrmale06.nif");
	someDraugr->SetAnimation("anims/draugr/alcove_wake.kf");
	//someDraugr = new Creature("DraugrRace", "actors\\dlc02\\hulkingdraugr\\hulkingdraugr.nif");
	//someDraugr->Place("gloomgendraugr");
	//meanSkelly = new BodyPart("DraugrRace", "actors\\draugr\\character assets\\draugrskeleton.nif");
	//meanSkelly->PutDown("gloomgenskeleton");
	someHuman = new Character();
	someHuman->Place("gloomgenman");
	someHuman->SetAnimation("anims/character/1hm_idle.kf");
	//player1 = new Player();
	program_while();
	return 1;
}

void dark::reload_esp()
{
	struct esp **plugin = &get_plugins()[5];
	free_plugin(plugin);
	*plugin = load_plugin(PLUGIN_5, true);
}

void dark::reload_dungeon()
{
	if (dungeon)
	{
		const char *edId = dungeon->edId;
		delete dungeon;
		dungeon = new Interior(edId);
		dungeon->alreadyTeleported = true;
		dungeon->Load();
	}
}

#include <skyrim/mesh.h>

#include <renderer/scene.h>
#include <renderer/group.h>
#include <renderer/drawgroup.h>

void dark::simple_viewer(const char *path)
{
	static Mesh *mesh = nullptr;
	static DrawGroup *drawGroup = nullptr;
	if (mesh)
	{
		drawGroup->parent->Remove(drawGroup);
		delete mesh;
		delete drawGroup;
	}
	mesh = new Mesh(path);
	drawGroup = new DrawGroup(
		mesh->baseGroup, translate(mat4(1.0), personCam->pos));
	sceneDef->bigGroup->Add(drawGroup);
	hide_cursor();
	cameraCur = viewerCam;
	viewerCam->pos = drawGroup->aabb.center();
	//viewerCam->pos = personCam->pos;
	viewerCam->radius = drawGroup->aabb.radius2() * 2;
}