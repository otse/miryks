#include "dark2.h"

#include "files"

extern "C" {
#include "c/c.h"
#include "c/bsa.h"
#include "c/nif.h"
}

#include "cpp/opengl/types"
#include "cpp/opengl/scene"
#include "cpp/opengl/group"
#include "cpp/opengl/geometry"
#include "cpp/opengl/material"

#include "cpp/mesh.h"

namespace dark2
{
	Mesh *viewed = nullptr;

	std::string OLDRIM;

	bsa_t *interface;
	bsa_t *meshes;
	bsa_t *animations;
	bsa_t *textures;

	Camera *camera;

	int width = 1920;
	int height = 1080;
	float delta = 1;
} // namespace dark2

#define PATH_TXT "path to oldrim.txt"

nif_t *dark2::nif_rc(rc_t *rc) {
	cassert_(rc, "mh no rc");
	bsa_read(rc);
	nif_t *nif = nif_alloc();
	nif->path = rc->path;
	nif->buf = rc->buf;
	nif_read(nif);
	nif_save(rc, nif);
	return nif;
}

void dark2::view_nif(rc_t *rc) {
	if (viewed)
	scene->Remove(dark2::viewed->base);
	Mesh *mesh = new Mesh;
	nif_t *nif = dark2::nif_rc(rc);
	mesh->Construct(nif);
	scene->Add(mesh->base);
	dark2::viewed = mesh;
}

int main()
{
	using namespace dark2;
	log_("dark2 loading");
	cassert_(exists(PATH_TXT), "missing " PATH_TXT);
	OLDRIM = fread(PATH_TXT);
	meshes = bsa_load((OLDRIM + "Data/Skyrim - Meshes.bsa").c_str());
	textures = bsa_load((OLDRIM + "Data/Skyrim - Textures.bsa").c_str());
	bsa_t *array[2] = { meshes, textures };
	bsas_add_to_loaded(&bsas, array, 2);
	programGo();
	oglGo();
	view_nif(bsa_find(dark2::meshes, "meshes\\clutter\\bucket02a.nif"));
	nif_test(meshes);
	programLoop();
	return 1;
}