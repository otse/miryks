#include <libs>
#include <Gloom/Gloom.h>
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

	const char *CURRENT_WRLD;
	const char *CURRENT_INTERIOR;

	std::string pathToOldrim;

	FirstPersonCamera *firstPersonCamera;
	ViewerCamera *panCamera;
	RenderTarget *renderRarget;

	int width = 2560;
	int height = 1800;
	float delta = 0;
} // namespace gloom

void customLoad()
{
	if (!exists_test3("custom.txt"))
		fwrite("custom.txt", "# put your plugins and archives here with line breaks\n");
	if (!exists_test3("path to oldrim.txt"))
		fwrite("path to oldrim.txt", "`C:/Program Files (x86)/Steam/steamapps/common/Skyrim/");
}

int main()
{
	using namespace gloom;
	CURRENT_WRLD = "Gloom";
	CURRENT_INTERIOR = "";
	customLoad();
	pathToOldrim = fread("path to oldrim.txt");
	cassert(exists((pathToOldrim + "TESV.exe").c_str()), "cant find tesv.exe");
	get_plugins()[0] = loadPlugin("Skyrim.esm");
	if (get_plugins()[0])
		printf("Phew, Loaded Skyrim.esm\n");
	get_plugins()[1] = loadPlugin("Gloom.esp");
	get_archives()[0] = loadArchive("Skyrim - Meshes.bsa");
	get_archives()[1] = loadArchive("Skyrim - Textures.bsa");
	get_archives()[2] = loadArchive("Skyrim - Animations.bsa");
	get_archives()[3] = loadArchive("HighResTexturePack01.bsa");
	get_archives()[4] = loadArchive("HighResTexturePack02.bsa");
	get_archives()[5] = loadArchive("HighResTexturePack03.bsa");
	programGo();
	firstPersonCamera = new FirstPersonCamera;
	panCamera = new ViewerCamera;
	openglScene();
	objs_init();
	setup_esc_menu();
	collision_init();
	cameraCurrent = firstPersonCamera;
#if 0
	// Secret bucket beginning
	Rc *rc = bsa_find_more("meshes\\clutter\\bucket02a.nif", 0x1);
	View(rc);
#endif
	//nif_test();
	nifp_test();
	dungeon = new Interior("GloomGen"); // <-- interior to load
	dungeon->loadCell();
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
