#include <skyrim_units>
#include <libs>

#include <Gloom/Util.h>

#include <Gloom/Mesh.h>
#include <Gloom/Object.h>

namespace gloom
{
	namespace util
	{
		Mesh *GrantMesh(Object &baseObject)
		{
			auto modl = baseObject.Data<const char *>("MODL", 0);
			if (!modl)
				return nullptr;
			Mesh *mesh = GetStoredMesh((void *)modl);
			if (mesh)
				return mesh;
			Rc *rc = LoadRc("meshes\\", modl, 0x1);
			if (!rc)
				return nullptr;
			Nif *nif = LoadNif(rc, true);
			mesh = new Mesh;
			mesh->Construct(nif);
			StoreMesh((void *)modl, mesh);
			return mesh;
		}
	}
};