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
	public:
	Mesh();

	Group baseGroup;
	Group *lastGroup;

	void Construct(nif_t *);
};

