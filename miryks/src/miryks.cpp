#include <miryks/miryks.h>
#include <miryks/miryks.hpp>

#include <miryks/model.h>
#include <miryks/player.h>

#include <opengl/camera.h>
#include <opengl/drawgroup.h>

const char *cells = Cells;
const char *races = Races;

namespace miryks
{
	interior *ginterior = nullptr;
	worldspace *gworldspace = nullptr;
	
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

	bool showCursor = false;

	char *editme;

	int init()
	{
		//itemfinder::init();
		return 0;
	}

	void load_data_files()
	{
		get_plugins()[0] = load_plugin(MASTER_0);
		get_plugins()[1] = load_plugin(MASTER_1);
		get_plugins()[2] = load_plugin(MASTER_2);
		get_plugins()[3] = load_plugin(MASTER_3);
		get_plugins()[4] = load_plugin(MASTER_4);
		get_plugins()[5] = load_plugin(PLUGIN_0, true);
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

	void third_person()
	{
		//if (player1)
		//	player1->toggleView();
	}

	void reload_plugin()
	{
		free_plugin(&get_plugins()[5]);
		get_plugins()[5] = load_plugin(PLUGIN_0, true);
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
		showCursor = false;
		cameraCur = viewerCam;
		viewerCam->pos = drawGroup->aabb.center();
		//viewerCam->pos = personCam->pos;
		viewerCam->radius = drawGroup->aabb.radius2() * 2;
	}

	Player *player1;
	Monster *someDraugr, *meanSkelly;
	Char *someHuman;
}