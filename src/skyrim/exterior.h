#pragma once

#include <dark/dark.h>
#include <dark/ref.h>

#include <skyrim/skyrim.h>
#include <skyrim/grup.h>
#include <skyrim/record.h>

using namespace dark;

namespace skyrim
{
	class Exterior
	{
	public:
		Exterior();
		~Exterior();

		void Load();
		void Unload();
	};

}