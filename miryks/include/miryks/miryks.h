#pragma once

#include <miryks/constants.h>
#include <miryks/libs.h>

#include <miryks/miryks.hpp>

#include <map>


namespace miryks
{
	namespace hooks
	{
	extern bool (*some_behavior)(int);
	}


	class keyframes;
	keyframes *load_keyframes_from_disk(const char *);
}