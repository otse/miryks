extern "C"
{	
#include <half.h>
}

#include "trash.h"

#include <dark/dark.h>

#include <core/files.h>
#include <imgui.h>

#include "../gooey/gooey.h"

#include <renderer/shader.h>
#include <renderer/texture.h>
#include <renderer/types.h>

void cont_menu();

namespace skyrim
{
	Cont *Cont::cur = nullptr;

	void Cont::Render()
	{
		cont_menu();
	}

	void Cont::Init()
	{
		unsigned int *coct = data<unsigned int *>("COCT");

		for (int i = 0; i < *coct; i++)
		{
			// container object
			struct Cnto
			{
				unsigned int item, count;
			};
			Cnto *cnto = data<Cnto *>("CNTO", i);
			Record wrcd = esp_get_form_id(cnto->item);
			if (wrcd.valid())
			{
				for (int j = 0; j < cnto->count; j++)
				{
					Item item(wrcd);
					items.push_back(item);
				}
			}
		}
	}
}

void cont_menu()
{
	ImGuiIO& io = ImGui::GetIO();

	io.DisplaySize.x;

	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration;
	ImGui::SetNextWindowSize(ImVec2(600, 600));
	ImGui::SetNextWindowPos(ImVec2(300, 300));

	ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));

	ImGui::Begin("Container Menu", nullptr, flags);

	ImGui::PushFont(dark::font2);
	ImGui::TextWrapped("Container");
	ImGui::Separator();

	const Cont *cont = Cont::cur;

	for (const Item &item : cont->items)
	{
		auto FULL = item.data<const char *>("FULL");
		auto EDID = item.data<const char *>("EDID");
		auto name = FULL ? FULL : EDID;
		ImGui::Text("%s", name);
	}

	ImGui::PopFont();

	ImGui::PopStyleColor(4);
	
	ImGui::End();
}