#include <lib.h>

#include "mesh.h"

#include <renderer/shader.h>
#include <renderer/texture.h>
#include <renderer/types.h>

using namespace dark;

namespace skyrim
{
	static void other(Rd *, void *);
	static void ni_node_callback						(Rd *, ni_node_pointer *);
	static void ni_node_callback_2						(Rd *, ni_node_pointer *);
	static void ni_tri_shape_callback					(Rd *, ni_tri_shape_pointer *);
	static void ni_tri_shape_callback_2					(Rd *, ni_tri_shape_pointer *);
	static void ni_tri_shape_data_callback				(Rd *, ni_tri_shape_data_pointer *);

}