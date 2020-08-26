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

using namespace dark2;

void loadBucket() {
	Mesh *mesh = new Mesh;
	rc_t *rc = bsa_find(dark2::meshes, "meshes\\clutter\\bucket02a.nif");
	cassert_(rc, "mh no bucket02a cc");
	bsa_read_rc(rc);
	nif_t *bucket = nif_alloc();
	bucket->path = rc->path;
	bucket->buf = rc->buf;
	nif_read(bucket);
	nif_save(rc, bucket);
	mesh->Construct(bucket);
	scene->Add(mesh->base);
}

int main()
{
	log_("dark2 loading");
	cassert_(exists(PATH_TXT), "missing " PATH_TXT);
	OLDRIM = fread(PATH_TXT);
	meshes = bsa_load((OLDRIM + "Data/Skyrim - Meshes.bsa").c_str());
	textures = bsa_load((OLDRIM + "Data/Skyrim - Textures.bsa").c_str());
	bsa_t *array[2] = { meshes, textures };
	bsas_add_to_loaded(&bsas, array, 2);
	programGo();
	oglGo();
	loadBucket();
	nif_test(meshes);
	programLoop();
	return 1;
}