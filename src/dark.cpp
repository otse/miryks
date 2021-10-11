#include <png.h>

#include <window.h>

#include <dark/dark.h>
#include <dark/files.h>

#include <dark/player.h>

#include <skyrim/cell.h>
#include <skyrim/actors.h>

#include <overlays/overlays.h>

using namespace dark;
using namespace skyrim;

#define READ_WHOLE true

namespace dark
{
Monster *someDraugr = nullptr, *meanSkelly = nullptr;
Char *someHuman = nullptr;
Player *player1 = nullptr;
std::map<const char *, int> keys;
}

namespace dark
{
void darkassert(bool e)
{
	assertc(e);
}

void view_bucket_in_place()
{
	view_in_place(get_res("clutter\\bucket02a.nif"));
}

void load_interior(const char *name)
{
	dungeon = get_interior(name)->Init();
}

void load_world_space(const char *name)
{
	worldSpace = get_world_space(name)->Init();
}

void third_person()
{
	if (player1)
		player1->toggleView();
}

void reload_plugin()
{
	free_plugin(&get_plugins()[5]);
	get_plugins()[5] = load_plugin(PLUGIN_5, READ_WHOLE);
}

void reload_dungeon()
{
	if (!dungeon)
		return;
	const char *edId = dungeon->edId;
	delete dungeon;
	dungeon = get_interior(edId);
	dungeon->dontTeleport = true;
	dungeon->Init();
}
}

void load_archives();

void load_data_files()
{
	get_plugins()[0] = load_plugin(PLUGIN_0);
	get_plugins()[1] = load_plugin(PLUGIN_1);
	get_plugins()[2] = load_plugin(PLUGIN_2);
	get_plugins()[3] = load_plugin(PLUGIN_3);
	get_plugins()[4] = load_plugin(PLUGIN_4);
	get_plugins()[5] = load_plugin(PLUGIN_5, READ_WHOLE);
	assertc(get_plugins()[0]);
	assertc(get_plugins()[1]);
	assertc(get_plugins()[2]);
	assertc(get_plugins()[3]);
	assertc(get_plugins()[4]);
	assertc(get_plugins()[5]);
	load_archives();
}

void load_archives()
{
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
	editme = get_text_file(EDIT_ME);
	load_data_files();
	nif_test();
	init();
	load_yagrum();
	renderer_init();
	view_bucket_in_place();
	yagrum_queue("", 10, true);
	refs_init();
	//load_world_space();
#if 1
	load_interior();
	someDraugr = new Monster("DraugrRace", "actors\\draugr\\character assets\\draugrfemale.nif");
	someDraugr->SetAnim("anims/draugr/alcove_wake.kf");
	someDraugr->Place("gloomgendraugr");
	//someDraugr = new Monster("DraugrRace", "actors\\dlc02\\hulkingdraugr\\hulkingdraugr.nif");
	//meanSkelly = new BodyPart("DraugrRace", "actors\\draugr\\character assets\\draugrskeleton.nif");
	//meanSkelly->PutDown("gloomgenskeleton");
	//someHuman = new Char();
	//someHuman->SetAnim("anims/character/idlewarmhands_crouched.kf");
	//someHuman->Place("gloomgenman");
#endif
	//someHuman->SetAnim("anims/character/1hm_idle.kf");
	player1 = new Player();
	window_while_test();
	return 1;
}

#include <skyrim/model.h>

#include <renderer/scene.h>
#include <renderer/group.h>
#include <renderer/drawgroup.h>

void dark::view_in_place(Res *res)
{
	static Model *model = nullptr;
	static DrawGroup *drawGroup = nullptr;
	if (model)
	{
		drawGroup->parent->Remove(drawGroup);
		delete model;
		delete drawGroup;
	}
	model = new Model(res);
	drawGroup = new DrawGroup(
		model->baseGroup, translate(mat4(1.0), personCam->pos));
	sceneDef->bigGroup->Add(drawGroup);
	now_you_dont();
	cameraCur = viewerCam;
	viewerCam->pos = drawGroup->aabb.center();
	//viewerCam->pos = personCam->pos;
	viewerCam->radius = drawGroup->aabb.radius2() * 2;
}