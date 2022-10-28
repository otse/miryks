#include <png.h>

#include <dark/dark.h>
#include <dark/window.h>
#include <dark/my_reference.h>

#include <miryks/player.h>
#include <miryks/actors.h>
#include <miryks/collision.h>
#include <miryks/sound.h>

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
	bool usePostProcessing = true;

	std::map<const char *, int> keys;

	void darkassert(bool e)
	{
		assertc(e);
	}

	void view_bucket_in_place()
	{
		view_in_place(get_res("clutter\\bucket02a.nif"));
	}

	
	void load_interior(const char *name, int plugin, bool place)
	{
		using top =
		grup_iter<0,
		grup_iter<2,
		grup_iter<3>>>;
		printf("load interior %s\n", name);
		record_with_grup rg = find_record_with_grup_by_id(name, top("CELL", plugin));
		if (ginterior)
			delete ginterior;
		ginterior = new interior(rg);
		reference_factory<my_reference> factory;
		factory.cell = ginterior;
		itemfinder::clear();
		ginterior->iter_both_subgroups(factory);
		if (place)
			place_at_level_start();
	}

	void load_world_space(const char *name, int plugin, bool place)
	{
		if (ginterior)
			delete ginterior;
		itemfinder::clear();
		record_with_grup rg = find_record_with_grup_by_id(name, grup_iter<0>("WRLD", plugin));
		gworldspace = new worldspace(rg);
		reference_factory<my_reference> factory;
		gworldspace->build_exteriors(factory);
		usePostProcessing = false;
		//if (place)
		//	place_at_level_start();
	}

	void place_at_level_start()
	{
		for (auto refer : ginterior->references)
		{
			record marker = esp_get_form_id(*refer->base());
			// if (marker.editor_id("COCMarkerHeading"))
			if (*refer->base() == 0x00000032) // coc marker heading
			{
				if (refer->editor_id() && strstr(refer->editor_id(), "spawn"))
				{
					float *data = refer->data<float *>("DATA");
					personCam->pos = reinterpret_vec3(data);
					personCam->yaw = reinterpret_vec3(data + 3).z;
					if (player1)
						player1->Teleport();
					break;
				}
			}
		}
	}

	Draugr *draugrAlcove = nullptr;
	Monster *meanSkelly = nullptr;
}

int main()
{
	using namespace dark;
	using namespace miryks;

	init_miryks();
	if (installed_path.size() == 0)
		return 1;
	init_data_files();
	init_archives();
	hooks::load_interior = load_interior;
	hooks::load_world_space = load_world_space;
	collision::init();
	nif_test();
	sounds_init();
	init_dark();
	load_yagrum();
	renderer_init();
	//view_bucket_in_place();
	//yagrum_queue("Press ESC to leave the bucket-viewer", 5, true);
	// refs_init();
	// load_world_space();

	load_interior("GloomGen", 5, true);
	//load_interior("HighHrothgar", 0, true);
	//place_at_level_start();
	/*
	draugrskeleton
	draugrmale01 - 07
	*/
	#if 1
	draugrAlcove = new DraugrAlcove("actors\\draugr\\character assets\\draugrmale01.nif");
	//draugrAlcove->SetAnim("anims/draugr/alcove_wake.kf");
	// draugrAlcove->anim->keyf->repeats = false;
	draugrAlcove->wearHelmet = false;
	draugrAlcove->Setup();
	draugrAlcove->Place("gloomgendraugr");
	// draugrAlcove = new Monster("DraugrRace", "actors\\dlc02\\hulkingdraugr\\hulkingdraugr.nif");
	//meanSkelly = new Monster("DraugrRace", "actors\\draugr\\character assets\\draugrskeleton.nif");
	//meanSkelly = new Monster("HagravenRace", "actors\\hagraven\\character assets\\hagraven.nif");
	//meanSkelly->SetAnim("anims/draugr/alcove_wake.kf");
	//meanSkelly->Place("gloomgenskeleton");
	#endif
	// someHuman = new Char();
	// someHuman->SetAnim("anims/character/idlewarmhands_crouched.kf");
	// someHuman->Place("gloomgenman");

	// someHuman->SetAnim("anims/character/1hm_idle.kf");
	player1 = new Player();
	place_at_level_start();
	window_while_test();
}