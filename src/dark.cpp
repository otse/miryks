#include <png.h>

#include <window.h>

#include <dark/dark.h>
#include <dark/files.h>

#include <dark/player.h>

#include <miryks/cell.h>
#include <miryks/actors.h>

#include <overlays/overlays.h>

using namespace dark;
using namespace miryks;

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

}

#include <opengl/camera.h>
#include <opengl/rendertarget.h>

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
	return 1;
}

#include <miryks/model.h>

#include <opengl/scene.h>
#include <opengl/group.h>
#include <opengl/drawgroup.h>

