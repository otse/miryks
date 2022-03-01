#include <png.h>

#include <dark/dark.h>
#include <dark/window.h>
#include <dark/reference.h>

#include <miryks/player.h>
#include <miryks/actors.h>

#include <opengl/camera.h>
#include <opengl/rendertarget.h>

#include <panels.h>

#include <files.h>

std::map<const char *, int> keys;

bool pressing_key(const char *id)
{
	return keys[id] == 1;
}

bool holding_key(const char *id)
{
	return keys[id] >= 1;
}

namespace dark
{
	std::map<const char *, int> keys;

	void darkassert(bool e)
	{
		assertc(e);
	}

	void view_bucket_in_place()
	{
		view_in_place(get_res("clutter\\bucket02a.nif"));
	}

	void load_interior(const char *name) {
		printf("(hook) load interior");
		
		if (ginterior)
			delete ginterior;

		ginterior = mir_dig_create_interior(name);

		reference_factory_iter<
			my_reference> factory;
		factory.cell = ginterior;

		ginterior->iter_both_subgroups(factory);
	}

	void putcam()
	{
		for (auto refer : ginterior->refers)
		{
			if (*refer->base() == 0x00000032) // coc marker heading
			{
				float *data = refer->data<float *>("DATA");
				personCam->pos = cast_vec3(data);
				personCam->pos.z += EYE_HEIGHT;
				personCam->yaw = cast_vec3(data + 3).z;
				break;
			}
		}
	}

	Monster *someDraugr = nullptr;
}

int main()
{
	using namespace dark;
	using namespace miryks;

	init_miryks();
	init_data_files();
	init_archives();

	hooks::hooks_load_interior = load_interior;
	
	nif_test();
	init_dark();

	load_yagrum();

	renderer_init();

	view_bucket_in_place();

	yagrum_queue("", 10, true);
	//refs_init();
	//load_world_space();
#if 1
	load_interior("GloomGen");

	putcam();
	someDraugr = new Monster("DraugrRace", "actors\\draugr\\character assets\\draugrskeleton.nif");
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
}

void miryks::view_in_place(resource *res)
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