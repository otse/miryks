#include "dark2.h"

#include "files"

extern "C" {
#include "c/bsa.h"
#include "c/nif.h"
}

#include "cpp/opengl/types"
#include "cpp/opengl/scene"
#include "cpp/opengl/group"
#include "cpp/opengl/geometry"
#include "cpp/opengl/material"


class Mesh {
	Mesh(nif_t *);

	nif_t *nif;

	Group baseGroup;

	void Construct();
};

