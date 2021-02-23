#include <libs>
#include <Gloom/Dark2.h>
#include <Gloom/Obj.h>

#include <algorithm>

#include <OpenGL/Types.h>
#include <OpenGL/Camera.h>
#include <OpenGL/Scene.h>
#include <OpenGL/Group.h>
#include <OpenGL/bound.h>
#include <OpenGL/Geometry.h>
#include <OpenGL/Material.h>

#include <Gloom/Files.h>

#include <Gloom/Interior.h>
#include <Gloom/Collision.h>
#include <Gloom/Mesh.h>
#include <Gloom/Actor.h>

namespace gloom
{
	unsigned int fps = 0;

	Interior *dungeon = nullptr;
	Actor *someDraugr = nullptr;
	Actor *meanSkelly = nullptr;
	Human *someHuman = nullptr;
	Player *player1 = nullptr;

	std::string pathToOldrim;

	FirstPersonCamera *first_person_camera;
	ViewerCamera *viewer_camera;
	RenderTarget *renderRarget;

	int width = 2560;
	int height = 1800;
	float delta = 1;
} // namespace gloom

int main()
{
	using namespace gloom;
	printf("loading\n");
	pathToOldrim = fread("path to oldrim.txt");
	cassert(exists("path to oldrim.txt"), "missing path to oldrim.txt");
	cassert(exists((pathToOldrim + "TESV.exe").c_str()), "cant find tesv.exe, check your path");
	get_plugins()[0] = LoadPlugin("Skyrim.esm");
	if (get_plugins()[0])
		printf("Phew, Loaded Skyrim.esm\n");
	get_plugins()[1] = LoadPlugin("Gloom.esp");
	get_archives()[0] = LoadArchive("Skyrim - Meshes.bsa");
	get_archives()[1] = LoadArchive("Skyrim - Textures.bsa");
	get_archives()[2] = LoadArchive("Skyrim - Animations.bsa");
	get_archives()[3] = LoadArchive("HighResTexturePack01.bsa");
	get_archives()[4] = LoadArchive("HighResTexturePack02.bsa");
	get_archives()[5] = LoadArchive("HighResTexturePack03.bsa");
	programGo();
	first_person_camera = new FirstPersonCamera;
	viewer_camera = new ViewerCamera;
	openglScene();
	objs_init();
	setup_esc_menu();
	collision_init();
	camera = first_person_camera;
#if 0
	// Secret bucket beginning
	Rc *rc = bsa_find_more("meshes\\clutter\\bucket02a.nif", 0x1);
	View(rc);
#endif
	//nif_test();
	nifp_test();
	dungeon = new Interior("GloomGen"); // <-- interior to load
	dungeon->LoadCell();
	//someDraugr = new Actor("DraugrRace", "meshes\\actors\\draugr\\character assets\\draugrmale.nif");
	//someDraugr->PutDown("gloomgendraugr");
	//meanSkelly = new Actor("DraugrRace", "meshes\\actors\\draugr\\character assets\\draugrskeleton.nif");
	//meanSkelly->PutDown("gloomgenskeleton");
	someHuman = new Human();
	someHuman->Place("gloomgenman");

	player1 = new Player();

	programLoop();
	return 1;
}
