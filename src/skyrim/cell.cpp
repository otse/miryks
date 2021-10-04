#include <skyrim_units>

#include <skyrim/skyrim.h>
#include <skyrim/cell.h>
#include <skyrim/model.h>
#include <skyrim/grup.h>

#include <algorithm>
#include <cctype>
#include <string>

#include <renderer/renderer.h>
#include <renderer/texture.h>
#include <renderer/camera.h>
#include <renderer/lights.h>

namespace skyrim
{
	Cell::Cell(Record r, Grup<> g) {
		record = r;
		persistent = g.get_grup();
		temporary = g.next().get_grup();
	}
	
	Exterior::Exterior()
	{
        Load();
	}

	Exterior::~Exterior()
	{
		Unload();
	}

	void Exterior::Load()
	{
		
	}

	void Exterior::Unload()
	{
        
	}
} // namespace dark