#include <string>
#include <deque>
#include <gloom/defs>

using namespace gloom;

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
	};

	extern entry *current;
	extern std::deque<entry> vector;

	void load();
	void queue(const char *q, float duration, bool rotate = false);
	void pop();
	void set_rotate_speed(float speed);
	void force_fade();
	void checker();
	void drawer();
}

static inline void yagrum_checker()
{
	yagrum::checker();
}

static inline void load_yagrum()
{
	yagrum::load();
}