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

	nif_t *nif;
	Group *base, *front;

	void Construct(nif_t *);
	Group *Nested(int);
};

