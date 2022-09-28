#include <miryks/miryks.hpp>

#include <miryks/model.h>
#include <miryks/player.h>

bool showCursor = false;

namespace miryks
{
	namespace hooks
	{
	bool (*hooks_some_behavior)(int) = 0;
	void (*load_interior)(const char *, bool) = 0;
	void (*load_world_space)(const char *, bool) = 0;
	}
	
	char *editme;

	std::map<const char *, nif *> nis;

	interior *ginterior = nullptr;
	worldspace *gworldspace = nullptr;

	Player *player1 = nullptr;
}

void miryks::view_in_place(resource *res)
{
	static Model *model = nullptr;
	static GroupDrawer *groupDrawer = nullptr;
	if (model)
	{
		groupDrawer->parent->Remove(groupDrawer);
		delete model;
		delete groupDrawer;
	}
	model = new Model(res);
	groupDrawer = new GroupDrawer(
		model->baseGroup, translate(mat4(1.0), personCam->pos));
	sceneDef->bigGroup->Add(groupDrawer);
	showCursor = false;
	cameraCur = viewerCam;
	viewerCam->pos = groupDrawer->aabb.center();
	// viewerCam->pos = personCam->pos;
	viewerCam->radius = groupDrawer->aabb.radius2() * 2;
}