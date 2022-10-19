#pragma once

#include <miryks/miryks.hpp>

namespace miryks {
	void sounds_init();
	class sound
	{
	public:
		unsigned int buffer = 0;
		unsigned int source = 0;
		sound(const char *);
		void play();
		bool is_playing();
	};
}