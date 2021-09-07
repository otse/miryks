#pragma once

#include <skyrim/grup.h>

namespace skyrim
{
	//extern std::map<int, int> keyframes;

	// unused / unusable
	
	class Record;
	class Grup;
	
	class Mesh;
	class SkinnedMesh;
	class Skeleton;
	class Bone;
	class Keyframes;
	class Animation;
	class Interior;
	class Container;

	struct Cell
	{
		Record wrcd;
		Grup persistent, temporary;
	};

	Record skyrim_get_race(const char *);

	Cell skyrim_get_interior_cell(const char *, int);

}