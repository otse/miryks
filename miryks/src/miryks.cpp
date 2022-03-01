#include <miryks/miryks.hpp>

#include <miryks/model.h>
#include <miryks/player.h>

bool showCursor = false;

namespace miryks
{
	namespace hooks
	{
		bool (*hooks_some_behavior)(int) = 0;
		void (*hooks_load_interior)(const char *) = 0;
	}
	
	char *editme;

	std::map<const char *, nif *> nis;

	interior *ginterior = nullptr;
	worldspace *gworldspace = nullptr;

	Player *player1 = nullptr;
}