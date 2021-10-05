#include <dark/dark.h>
#include <dark/files.h>

#include <skyrim/grup.h>
#include <skyrim/record.h>

extern "C"
{
#include "common.h"
#include "esp.h"
}

using namespace dark;

#include <sstream>
#include <imgui.h>

static std::stringstream ss;

static Esp *plugin = NULL;

void im_grup(GRUP, int);
void im_record(RCD);
void im_subrecord(SRCD);

void im_grup(GRUP grp, int top_grup = -1)
{
	Grup wgrp = grp; // invokes unlooped grup
	char t[100];
	snprintf(t, 100, "GRUP %i %.4s", grp->id, (char *)&grp->hed->label);
	if (ImGui::TreeNode(t))
	{
		char s[100];
		esp_print_grup(plugin, s, grp);
		ImGui::Text(s);
		for (unsigned int i = 0; i < grp->mixed->size; i++)
		{
			void *element = grp->mixed->elements[i];
			if (*(char *)element == 'g')
				im_grup((GRUP)grp->mixed->elements[i]);
			if (*(char *)element == 'r')
				im_record((RCD)grp->mixed->elements[i]);
		}
		if (grp->mixed->size)
			ImGui::Separator();
		ImGui::TreePop();
	}
}

void im_record(RCD rcd)
{
	char *edid = nullptr;
	Record wrcd = rcd; // invokes read partials
	SRCD first = (SRCD)rcd->rcdbs->elements[0];
	if (first->hed->sgn == *(unsigned int *) "EDID")
	{
		edid = (char *)first->data;
	}
	char t[130];
	snprintf(t, 130, "%.4s %i - %s", (char *)&rcd->hed->sgn, rcd->id, edid);
	if (ImGui::TreeNode(t))
	{
		char s[200];
		esp_print_record(plugin, s, rcd);
		ImGui::Text(s);
		if (ImGui::TreeNode("formId"))
		{
			//if (rcd.form_id)
			{
				char s[200];
				esp_print_form_id(plugin, s, &rcd->form_id);
				ImGui::Text(s);
			}
			ImGui::TreePop();
		}
		for (unsigned int i = 0; i < rcd->rcdbs->size; i++)
		{
			im_subrecord((SRCD)rcd->rcdbs->elements[i]);
		}
		ImGui::TreePop();
	}
}

void im_subrecord(SRCD field)
{
	char t[100];
	snprintf(t, 100, "%.4s##Sub %i", (char *)&field->hed->sgn, field->id);
	if (ImGui::TreeNode(t))
	{
		char s[400];
		esp_print_field(plugin, s, field);
		ImGui::Text(s);
		ImGui::TreePop();
	}
}

void overlay_plugins()
{
	ImGuiIO &io = ImGui::GetIO();

	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize; // | ImGuiWindowFlags_NoSavedSettings;
	ImGui::SetNextWindowSize(ImVec2(450, io.DisplaySize.y));
	ImGui::SetNextWindowPos(ImVec2(450 * 2, 0));

	ImGui::Begin("Plugin", nullptr, flags);

	static const char *items[] = {
		PLUGIN_0,
		PLUGIN_1,
		PLUGIN_2,
		PLUGIN_3,
		PLUGIN_4,
		PLUGIN_5
	};

	static int current = 5;
	static int old = -1;

	ImGui::Combo("plugins", &current, items, IM_ARRAYSIZE(items));
	//ImGui::ListBox("##plugins", &current, items, num, 3);

	if (old != current)
	{
		plugin = get_plugins()[current];
		old = current;
	}

	ImGui::Separator();

	static revised_array * filtered = NULL;

	//if (plugin)
	//{
	//	ImGui::Text(plugin->name);
	//}

	if (!plugin)
	{
		ImGui::End();
		return;
	}

	ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;

	if (ImGui::BeginTabBar("EspTabs", tabBarFlags))
	{
		if (ImGui::BeginTabItem("plain"))
		{
			auto vec = ImVec2(0, ImGui::GetContentRegionAvail().y);
			const ImGuiWindowFlags child_flags = 0;
			const ImGuiID child_id = ImGui::GetID((void *)(intptr_t)0);
			const bool child_is_visible = ImGui::BeginChild(child_id, vec, true, child_flags);
			//if (ImGui::TreeNode("Header"))
			//{
			im_record(plugin->header);
			//ImGui::TreePop();
			//}
			//if (ImGui::TreeNode("Grups"))
			//{
			for (unsigned int i = 0; i < plugin->grups->size; i++)
				im_grup((GRUP)plugin->grups->elements[i], i);
			//ImGui::TreePop();
			//}
			ImGui::EndChild();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("filter"))
		{
			static char buf[5] = "NPC_";
			static char buf2[5] = {'\0'};

			ImGui::InputText("##espfilter", buf, 5);

			if (strcmp(buf, buf2))
			{
				printf("esp try filter %.4s\n", buf2);
				memcpy(buf2, buf, 5);
				if (filtered)
				{
					free_esp_array(&filtered);
					filtered = NULL;
				}
				if (strlen(buf2) == 4)
				{
					filtered = esp_filter_objects(plugin, buf2);
				}
			}
			if (filtered)
			{
				ImGui::Text("Found %i records", filtered->size);
				auto vec = ImVec2(0, ImGui::GetContentRegionAvail().y);
				//ImGui::Text("%i", esp->statics.size);
				const ImGuiWindowFlags child_flags = 0;
				const ImGuiID child_id = ImGui::GetID((void *)(intptr_t)0);
				const bool child_is_visible = ImGui::BeginChild(child_id, vec, true, child_flags);
				for (unsigned int i = 0; i < filtered->size; i++)
					im_record((record *)filtered->elements[i]);
				ImGui::EndChild();
			}
			ImGui::EndTabItem();
		}
		/*if (ImGui::BeginTabItem("statics"))
		{
			//ImGui::Text("%i", esp->statics.size);
			const ImGuiWindowFlags child_flags = 0;
			const ImGuiID child_id = ImGui::GetID((void*)(intptr_t)0);
			const bool child_is_visible = ImGui::BeginChild(child_id, ImVec2(0, 600), true, child_flags);
			for (int i = 0; i < esp->statics.size; i++)
				im_record((record *)esp->statics.elements[i]);
			ImGui::EndChild();
			ImGui::EndTabItem();
		}*/
		/*if (ImGui::BeginTabItem("formIds"))
		{
#if 0
			//ImGui::Text("%i", esp->formIds.size);
			const ImGuiWindowFlags child_flags = 0;
			const ImGuiID child_id = ImGui::GetID((void*)(intptr_t)0);
			const bool child_is_visible = ImGui::BeginChild(child_id, ImVec2(0, 600), true, child_flags);
			for (int i = 0; i < esp->formIds.size; i++)
			{
				struct form_id *formId = &esp->formIds.formIds[i];
				if (ImGui::TreeNode(formId->hex))
				{

					ImGui::TreePop();
				}
			}
			ImGui::EndChild();
#endif
			ImGui::EndTabItem();
		}*/
		ImGui::EndTabBar();
	}

	ImGui::End();
}