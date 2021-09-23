#pragma once

#include <dark/dark.h>
#include <dark/ref.h>

#include <skyrim/skyrim.h>
#include <skyrim/grup.h>
#include <skyrim/record.h>

using namespace dark;

namespace skyrim
{
	class SKInterior
	{
	public:
		SKInterior(const char *);
		~SKInterior();

		SKCell cell;
		std::vector<Ref *> refs, labels, mstts;
		std::map<std::string, Ref *> edIds;

		void Update();
		void Subgroup(SKGrup, int);
		void Load();
		void Unload();
		void put_cam_on_random_xmarker();

		const char *edId = nullptr;
		bool alreadyTeleported = false;
	};

}