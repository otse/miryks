#include <skyrim_units>
#include <libs>
#include <Gloom/Util.h>

#include <Gloom/Mesh.h>
#include <Gloom/Object.h>

namespace gloom
{
	Mesh *GrantMesh(Object &baseObject)
	{
		Mesh *mesh = nullptr;
		auto modl = baseObject.Data<const char *>("MODL", 0);
		if (!modl)
			return nullptr;
		mesh = Mesh::Cached((void *)modl);
		if (!mesh)
		{
			Rc *rc = LoadRc("meshes\\", modl, 0x1);
			if (rc)
			{
				Nif *nif = LoadNif(rc, true);
				mesh = new Mesh;
				mesh->Construct(nif);
				Mesh::Store((void *)modl, mesh);
			}
			else
				printf("no rc for refr-modl\n");
		}
		return mesh;
	}
};