#include "gooey.h"
#include "yagrum.h"

#include <gloom/image.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <imgui.h>

#include <windows.h>

namespace yagrum
{
	Image *image = nullptr;

	std::deque<entry> vector;

	static const float fade_time = 1.0;

	static float fade = 1.0f;

	entry *current = nullptr;

	void load()
	{
		image = new Image();
		image->from_resourcefile();
	}

	static void start_new_yagrum()
	{
		if (vector.size() < 1)
			return;
		entry *yagrum = &(vector.front());
		yagrum->start_time = glfwGetTime();
		current = yagrum;
		fade = 1.0f;
		printf("starting yagrum %s\n", yagrum->quote.c_str());
	}

	void queue(const char *q, float duration, bool rotate)
	{
		entry talk;
		talk.quote = std::string(q);
		talk.duration = duration;
		talk.rotate = rotate;
		vector.push_back(talk);
	}

	void set_no_image(bool n)
	{
		(&vector.back())->no_image = n;
	}

	void set_rotate_speed(float n)
	{
		(&vector.back())->rotate_speed = n;
	}

	void pop()
	{
		vector.pop_front();
		current = nullptr;
	}

	void force_fade()
	{
		current->start_time = glfwGetTime() - current->duration;
	}

	void checker()
	{
		if (!vector.size())
			return;
		if (current && current->faded)
		{
			printf("popping yagrum\n");
			pop();
			current = nullptr;
		}
		if (!current)
			start_new_yagrum();
		drawer();
	}

	void drawer()
	{
		using namespace gloom;

		if (!current)
			return;

		ImGuiIO &io = ImGui::GetIO();

		io.DisplaySize.x;

		ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration;
		ImGui::SetNextWindowSize(ImVec2(500, 100));
		ImGui::SetNextWindowPos(ImVec2(300, 300));

		ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));

		ImGui::Begin("Yagrum", nullptr, flags);

		ImGui::PushFont(font2);

		bool should_be_fading = glfwGetTime() - current->start_time > current->duration;
		bool faded = glfwGetTime() - current->start_time > current->duration + fade_time;

		current->faded = faded;

		if (should_be_fading)
		{
			fade -= 1.0f / 60.0f;
			fade = max(0.f, fade);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, fade);
			if (fade <= 0.f)
				current->faded = true;
		}

		ImGui::TextWrapped(current->quote.c_str());
		ImGui::SameLine();
		if (current->no_image == false)
		{
			if (current->rotate)
			{
				ImRotateStart();
				ImGui::Image((void *)(intptr_t)image->texture, ImVec2(50, 50));
				ImRotateEnd(0.0005f * current->rotate_speed * ::GetTickCount(), ImRotationCenter());
			}
			else
			{
				ImGui::Image((void *)(intptr_t)image->texture, ImVec2(50, 50));
			}
		}
		ImGui::PopFont();

		if (should_be_fading)
			ImGui::PopStyleVar();

		ImGui::PopStyleColor(4);

		ImGui::End();
	}
}