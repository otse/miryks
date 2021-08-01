#include <string>
#include <deque>
#include <dark/fwd>

using namespace dark;

namespace yagrum
{
	extern Image *image;

	struct entry
	{
		std::string quote;
		float duration;
		float start_time = 0;
		bool faded = false;
		bool rotate = false;
		float rotate_speed = 1.f;
		bool no_image = false;
	};

	extern entry *current;
	extern std::deque<entry> vector;
}

void load_yagrum();

void yagrum_queue(const char *q, float duration, bool rotate = false);
void yagrum_pop();
void yagrum_set_rotate_speed(float = 1.0);
void yagrum_set_no_image(bool = true);
void yagrum_force_fade();
void yagrum_checker();
void yagrum_drawer();