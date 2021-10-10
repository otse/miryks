#pragma once
#include <string>
#include <deque>
#include <vector>

#include <dark/image.h>

#include <imgui.h>

extern ImFont *font2;
extern ImFont *font3;

void ImRotateStart();
ImVec2 ImRotationCenter();
void ImRotateEnd(float, ImVec2);

namespace yagrum
{
	extern dark::Image *image;

	struct entry
	{
		std::string quote;
		float duration;
		float start_time = 0;
		bool faded = false;
		bool rotate = false;
		float rotate_speed = 3.f;
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