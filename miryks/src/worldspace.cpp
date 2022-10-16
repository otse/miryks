#include <miryks/miryks.hpp>

#include <algorithm>
#include <cctype>
#include <string>

#include <opengl/renderer.h>
#include <opengl/camera.h>
#include <opengl/scene.h>

namespace miryks
{
	struct exterior_iter : record_with_grup_iter
	{
		worldspace *ws;
		bool fat_arrow(exterior_iter &target)
		{
			exterior *cell = new exterior(*this);
			target.ws->exteriors.push_back(cell);
			return false;
		}
	};

	struct careful_wrld_parser : unknown_iter
	{
		record last_cell;
		worldspace *ws;
		bool fat_arrow(careful_wrld_parser &rhs)
		{
			if (is_record())
			{
				record r = as_record();
				rhs.last_cell = r;
				//printf("this must be a CELL %.4s\n", (char *)&r.rhed().sgn);
			}
			if (is_grup())
			{
				grup g = as_grup();
				if (g.ghed().group_type == cell_children)
				{
					record_with_grup rg;
					rg.r = rhs.last_cell.r;
					rg.g = g.g;
					exterior *cell = new exterior(rg);
					rhs.ws->exteriors.push_back(cell);
				}
				//printf("this is a grup of type %i\n", g.ghed().group_type);
			}
			return false;
		}
	};

	struct land_iter : record_iter
	{
		exterior *cell = nullptr;
		bool fat_arrow(land_iter &rhs)
		{
			if (!this->is_type("LAND"))
				return false;
			printf("land iter\n");
			land *lan = new land(*this, rhs.cell);
			// lan->exterior = rhs.cell;
			return false;
		}
	};

	void worldspace::init()
	{
		auto type =
		grup_iter<1,
		grup_iter<4,
		grup_iter<5>>>();
		type = childs;
		// exterior_iter factory;
		careful_wrld_parser factory;
		factory.ws = this;
		printf("going to iter for wrld exteriors\n");
		type <= factory;

		sceneDef->ambient = vec3(127.f / 255.f);
		personCam->pos = vec3(0, 0, -2048.0);
		// make();
	}

	// void exterior::make() {
	// }

	void exterior::first_make_land()
	{
		land_iter factory;
		factory.cell = this;
		iter_both_subgroups(factory);
	}
}

#if 0
namespace miryks
{
#if 0
	mir_worldspace *get_world_space(const char *id, int num)
	{
		printf("get_world_space\n");
		mir_worldspace *worldSpace = nullptr;
#if 0
		constellation copy;
		bool ok = constellation::iter(WRLD).loop([&](constellation &temp) {
			return (copy=temp).self.editor_id(id);
		}, 0);
		if (ok)
			worldSpace = new mir_worldspace(copy);
#endif
		return worldSpace;
	}
#endif

	worldspace::worldspace(record_with_grup_iter rg) : record(rg)
	{
		childs = rg;
		assertc(childs.ghed().Group == world_children);
		sceneDef->ambient = vec3(127.f / 255.f);
		printf("new mir_worldspace: %s\n", data<const char *>("FULL"));
		formId xlcn = data<formId>("XLCN");
		int16_t *wctr = data<int16_t *>("WCTR");
		int32_t *nam0 = data<int32_t *>("NAM0");
		int32_t *nam9 = data<int32_t *>("NAM9");
	}

	mir_worldspace *mir_worldspace::Init()
	{
		sceneDef->ambient = vec3(180.0 / 255.0);
		personCam->pos = vec3(0, 0, -2048.0);
		DiscoverAllCells();
		LoadExterior(0, 0);
		return this;
	}

	void mir_worldspace::DiscoverAllCells()
	{
#if 0
		printf("DiscoverAllCells\n");
		childs.index = 2;
		constellation temp;
		childs.dive([&](constellation &temp) {
			mir_exterior *exterior = new mir_exterior(temp);
			exterior->worldSpace = this;
			exteriors.push_back(exterior);
			return false;
		}, {
			WorldChildren,
			ExteriorCellBlock,
			ExteriorCellSubBlock
		});
#endif
	}

	void mir_worldspace::LoadExterior(int x, int y)
	{
		for (mir_exterior *exterior : exteriors)
		{
			if (exterior->xclc->x == x && exterior->xclc->y == y)
			{
				printf("loading exterior %i %i\n", x, y);
				exterior->Init();
			}
		}
	}

	mir_exterior::mir_exterior(rgrup_copy rg) : cell(rg)
	{
		worldSpace = nullptr;
		land = nullptr;
		xclc = data<XCLC *>("XCLC");
		assertc(xclc);
	}

	void mir_exterior::Init()
	{
		//pt_ignore target;
		//persistent <= target;
		//temporary <= target;
	}

#if 0
	old_function
	{
		if (!subgroup.valid()) {
			return;
		}
		subgroup.loop([&](any &temp) {
			record refr = temp.u.r;
			if (refr.is_type(REFR))
			{
				reference *reference = new reference(refr);
				reference->cell = (cell *)this;
				worldSpace->references.push_back(reference);
				if (refr.editor_id("darkworldheading"))
				{
					float *loc = refr.data<float *>("DATA");
					personCam->pos = reinterpret_vec3(loc);
					personCam->pos.z += EYE_HEIGHT;
					personCam->yaw = reinterpret_vec3(loc + 3).z;
					cameraCur = personCam;
				}
			}
			else if (refr.is_type(LAND))
			{
				assertc(land == nullptr);
				land = new land(refr);
				land->exterior = this;
			}
			return false;
		}, Group);
	}
#endif

}
#endif