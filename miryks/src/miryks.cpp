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
	std::map<const char *, nif *> nis;

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

	void view_in_place(resource *res)
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