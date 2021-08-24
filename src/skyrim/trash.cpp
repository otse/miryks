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
	Cont *Cont::current = nullptr;

	void Cont::Render()
	{
		cont_menu();
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

	const Cont *cont = Cont::current;
	
	// container count
	unsigned int *coct = cont->wrcd.data<unsigned int *>("COCT");

	if (!*coct)
	{
		ImGui::TextWrapped("< Empty >");
	}
	else
	{
		ImGui::TextWrapped("Number of items: %u", *coct);

		for (int i = 0; i < *coct; i++)
		{
			// container object
			struct Cnto {
				unsigned int item, count;
			};
			Cnto *cnto = cont->wrcd.data<Cnto *>("CNTO", i);
			Record item = esp_get_form_id(cnto->item);
			if (item.valid())
			{
				const char *EDID = item.data<const char *>("EDID");
				const char *FULL = item.data<const char *>("FULL");
				if (EDID)
				ImGui::TextWrapped("%s", EDID);
			}
		}
	}


	ImGui::PopFont();

	ImGui::PopStyleColor(4);
	
	ImGui::End();
}