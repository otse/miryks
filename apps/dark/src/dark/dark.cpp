#include <png.h>

#include <dark/dark.h>
#include <dark/window.h>
#include <dark/reference.h>

#include <miryks/player.h>
#include <miryks/actors.h>

#include <panels.h>

#include <files.h>

using namespace dark;
using namespace miryks;

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
}

#include <opengl/camera.h>
#include <opengl/rendertarget.h>

int main()
{
	editme = get_text_file(EDIT_ME);
	miryks::init();
	miryks::init_data_files();
	nif_test();
	dark::init();
	load_yagrum();
	renderer_init();
	view_bucket_in_place();
	yagrum_queue("", 10, true);
	//refs_init();
	//load_world_space();
#if 1
	ginterior = try_create_interior_instance("GloomGen");
	ginterior->iter<my_reference>();
	//someDraugr = new Monster("DraugrRace", "actors\\draugr\\character assets\\draugrskeleton.nif");
	//someDraugr->SetAnim("anims/draugr/alcove_wake.kf");
	//someDraugr->Place("gloomgendraugr");
	//someDraugr = new Monster("DraugrRace", "actors\\dlc02\\hulkingdraugr\\hulkingdraugr.nif");
	//meanSkelly = new BodyPart("DraugrRace", "actors\\draugr\\character assets\\draugrskeleton.nif");
	//meanSkelly->PutDown("gloomgenskeleton");
	//someHuman = new Char();
	//someHuman->SetAnim("anims/character/idlewarmhands_crouched.kf");
	//someHuman->Place("gloomgenman");
#endif
	//someHuman->SetAnim("anims/character/1hm_idle.kf");
	//player1 = new Player();
	window_while_test();
	return 1;
}