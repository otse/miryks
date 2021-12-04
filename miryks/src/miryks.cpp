#include <miryks/miryks.h>

#include <miryks/grup.h>
#include <miryks/record.h>
#include <miryks/cell.h>
#include <miryks/model.h>
#include <miryks/player.h>

#include <opengl/camera.h>
#include <opengl/drawgroup.h>

namespace miryks
{
	namespace hooks
	{
	bool (*some_behavior)(int) = 0;
	}

	namespace input
	{
	std::map<const char *, int> keys;
	bool pressing_key(const char *id)
	{
		return keys[id] == 1;
	}
	bool holding_key(const char *id)
	{
		return keys[id] >= 1;
	}
	}

	char *editme;

	int init()
	{
		return 0;
	}

	void load_data_files()
	{
		get_plugins()[0] = load_plugin(PLUGIN_0);
		get_plugins()[1] = load_plugin(PLUGIN_1);
		get_plugins()[2] = load_plugin(PLUGIN_2);
		get_plugins()[3] = load_plugin(PLUGIN_3);
		get_plugins()[4] = load_plugin(PLUGIN_4);
		get_plugins()[5] = load_plugin(PLUGIN_5, true);
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

	record get_race(const char *raceId, int num)
	{
		closure<record_with_id> closure(raceId);
		(grup<>(races, num)) <= closure;
		return closure.match;
	}

	void load_interior(const char *name)
	{
		dungeon = get_interior(name, 5)->Init();
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
		get_plugins()[5] = load_plugin(PLUGIN_5, true);
	}

	void reload_dungeon()
	{
		if (!dungeon)
			return;
		const char *edId = dungeon->edId;
		delete dungeon;
		dungeon = get_interior(edId, 5);
		dungeon->dontTeleport = true;
		dungeon->Init();
	}

	void view_in_place(Res *res)
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
		//now_you_dont();
		cameraCur = viewerCam;
		viewerCam->pos = drawGroup->aabb.center();
		//viewerCam->pos = personCam->pos;
		viewerCam->radius = drawGroup->aabb.radius2() * 2;
	}

	Player *player1;
	Monster *someDraugr, *meanSkelly;
	Char *someHuman;
}