#include <gloom/dark2.h>

#include <algorithm>

#include <libs>

#include <opengl/types.h>
#include <opengl/camera.h>
#include <opengl/scene.h>
#include <opengl/group.h>
#include <opengl/bound.h>
#include <opengl/geometry.h>
#include <opengl/material.h>

#include <gloom/files.h>

#include <gloom/level.h>
#include <gloom/mesh.h>
#include <gloom/actor.h>

namespace gloom
{
	namespace viewer
	{
		Mesh *mesh = nullptr;
		DrawGroup *drawGroup = nullptr;
	} // namespace viewer

	unsigned int fps = 0;

	Level *dungeon = nullptr;
	Actor *someDraugr = nullptr;
	Actor *meanSkelly = nullptr;
	Human *someHuman = nullptr;
	Player *player1 = nullptr;

	std::string pathToOldrim;

	FirstPersonCamera *first_person_camera;
	ViewerCamera *viewer_camera;
	RenderTarget *render_target;

	int width = 2560;
	int height = 1440;
	float delta = 1;
} // namespace gloom

namespace gloom
{
	vec2 *cast_vec_2(float *f) { return reinterpret_cast<vec2 *>(f); }
	vec3 *cast_vec_3(float *f) { return reinterpret_cast<vec3 *>(f); }
	vec4 *cast_vec_4(float *f) { return reinterpret_cast<vec4 *>(f); }
	mat3 *cast_mat_3(float *f) { return reinterpret_cast<mat3 *>(f); }
	mat4 *cast_mat_4(float *f) { return reinterpret_cast<mat4 *>(f); }

	void viewer::view(Rc *rc)
	{
		if (mesh)
		{
			scene->Remove(drawGroup);
			delete mesh;
			delete drawGroup;
		}
		Nifp *nif = nifp_saved(rc);
		if (nif == NULL)
		{
			nif = loadNifp(rc, 1);
			nifp_save(rc, nif);
		}
		mesh = new Mesh;
		mesh->Construct(nif);
		drawGroup = new DrawGroup(mesh->baseGroup, glm::translate(mat4(1.0), first_person_camera->pos));
		scene->Add(drawGroup);
		HideCursor();
		camera = viewer_camera;
		viewer_camera->disabled = false;
		viewer_camera->pos = drawGroup->aabb.center();
		//viewer_camera->pos = first_person_camera->pos;
		viewer_camera->radius = drawGroup->aabb.radius2() * 2;
	}
} // namespace gloom

int main()
{
	using namespace gloom;
	printf("loading\n");
	pathToOldrim = fread("path to oldrim.txt");
	cassert(exists("path to oldrim.txt"), "missing path to oldrim.txt");
	cassert(exists((pathToOldrim + "TESV.exe").c_str()), "cant find tesv.exe, check your path");
	get_plugins()[0] = loadEsp("Skyrim.esm");
	if (get_plugins()[0])
		printf("Ok, loaded Skyrim.esm wooo\n");
	get_plugins()[1] = loadEsp("Gloom.esp");
	get_archives()[0] = loadBsa("Skyrim - Meshes.bsa");
	get_archives()[1] = loadBsa("Skyrim - Textures.bsa");
	get_archives()[2] = loadBsa("Skyrim - Animations.bsa");
	//get_archives()[3] = loadBsa("HighResTexturePack01.bsa");
	//get_archives()[4] = loadBsa("HighResTexturePack02.bsa");
	//get_archives()[5] = loadBsa("HighResTexturePack03.bsa");
	programGo();
	first_person_camera = new FirstPersonCamera;
	viewer_camera = new ViewerCamera;
	openglScene();
	camera = first_person_camera;
#if 0
	rc *rc = bsa_find_more("meshes\\clutter\\bucket02a.nif", 0x1);
	viewer::view(rc);
#endif
	//nif_test();
	nifp_test();
	dungeon = new Level("GloomGen"); // <-- interior to load
	someDraugr = new Actor("DraugrRace", "meshes\\actors\\draugr\\character assets\\draugrmale.nif");
	someDraugr->PutDown("gloomgendraugr");
	meanSkelly = new Actor("DraugrRace", "meshes\\actors\\draugr\\character assets\\draugrskeleton.nif");
	meanSkelly->PutDown("gloomgenskeleton");
	someHuman = new Human();
	someHuman->Place("gloomgenman");

	//player1 = new Player();

	programLoop();
	return 1;
}
