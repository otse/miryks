#include "dark2.h"
#include "files.h"

extern "C"
{
#include "putc.h"
#include "esp.h"
}

using namespace dark2;

#include <sstream>
#include <imgui.h>

#define ESP_GUI "esp"

static stringstream ss;

static esp *plugin = NULL;

void im_grup(grup *, int);
void im_record(record *);
void im_subrecord(subrecord *);

void im_grup(grup *grup, int top_grup = -1)
{
	char t[100];
	snprintf(t, 100, "GRUP %i %s", grup->id, top_grup != -1 ? plugin->tops[top_grup] : "");
	if (ImGui::TreeNode(t))
	{
		char s[100];
		esp_print_grup(plugin, s, grup);
		ImGui::Text(s);
		for (int i = 0; i < grup->mixed.size; i++)
		{
			void *element = grup->mixed.elements[i];
			if (*(char *)element == GRUP)
				im_grup(grup->mixed.grups[i]);
			if (*(char *)element == RECORD)
				im_record(grup->mixed.records[i]);
		}
		if (grup->mixed.size)
			ImGui::Separator();
		ImGui::TreePop();
	}
}

void im_record(record *record)
{
	char *edid = nullptr;
	subrecord *first = record->fields.subrecords[0];
	if (first->hed->type == espwrd "EDID")
	{
		edid = (char *)first->data;
	}
	char t[130];
	snprintf(t, 130, "%.4s %i - %s", (char *)&record->hed->type, record->id, edid);
	if (ImGui::TreeNode(t))
	{
		char s[200];
		esp_print_record(plugin, s, record);
		ImGui::Text(s);
		if (ImGui::TreeNode("formId"))
		{
			if (record->fi)
			{
				char s[200];
				esp_print_form_id(plugin, s, record->fi);
				ImGui::Text(s);
			}
			ImGui::TreePop();
		}
		for (int i = 0; i < record->fields.size; i++)
		{
			im_subrecord(record->fields.subrecords[i]);
		}
		ImGui::TreePop();
	}
}

void im_subrecord(subrecord *field)
{
	char t[100];
	snprintf(t, 100, "%.4s##Sub %i", (char *)&field->hed->type, field->id);
	if (ImGui::TreeNode(t))
	{
		char s[400];
		esp_print_field(plugin, s, field);
		ImGui::Text(s);
		ImGui::TreePop();
	}
}

void esp_gui()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize; // | ImGuiWindowFlags_NoSavedSettings;
	ImGui::SetNextWindowPos(ImVec2(450, 0));

	ImGui::SetNextWindowSize(ImVec2(450, 0));
	ImGui::Begin(ESP_GUI, nullptr, flags);

	static bool pluginUsedByGame = false;
	static char buf[260] = "Skyrim.esm";
	static char buf2[260] = {'\0'};
	static char temporaryName[260] = {'\0'};
	ImGui::InputText("##archive", buf, 260);

	if (strcmp(buf, buf2))
	{
		memcpy(buf2, buf, 260);
		esp *plugin2 = has_plugin(buf);
		if (plugin2)
		{
			pluginUsedByGame = true;
			plugin = plugin2;
		}
		else
		{
			plugin2 = loadPlugin(buf2);
			if (plugin2)
			{
				memcpy(temporaryName, buf, 260);
				pluginUsedByGame = false;
				plugin = plugin2;
				plugin->name = temporaryName;
			}
		}
	}

	ImGui::Separator();

	if (plugin)
	{
		ImGui::Text(plugin->name);
		ImGui::TextWrapped(pluginUsedByGame ? "This plugin is active" : "Plugin loaded via imgui, but not active");
	}

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
			const ImGuiWindowFlags child_flags = 0;
			const ImGuiID child_id = ImGui::GetID((void *)(intptr_t)0);
			const bool child_is_visible = ImGui::BeginChild(child_id, ImVec2(0, 600), true, child_flags);
			//if (ImGui::TreeNode("Header"))
			//{
			im_record(plugin->header);
			//ImGui::TreePop();
			//}
			//if (ImGui::TreeNode("Grups"))
			//{
			for (int i = 0; i < plugin->grups.size; i++)
				im_grup((grup *)plugin->grups.elements[i], i);
			//ImGui::TreePop();
			//}
			ImGui::EndChild();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("filter"))
		{
			static esp_array *filtered = NULL;
			static char filter[5] = "NPC_";
			static char buf[5] = {'\0'};
			ImGui::InputText("##espfilter", filter, 5);
			if (strcmp(filter, buf))
			{
				printf("esp try filter %.4s\n", filter);
				memcpy(buf, filter, 4);
				if (filtered)
				{
					free_esp_array(filtered);
					filtered = NULL;
				}
				if (strlen(filter) == 4)
				{
					filtered = esp_filter_objects(plugin, filter);
				}
			}
			if (filtered)
			{
				ImGui::Text("Found %i records", filtered->size);
				//ImGui::Text("%i", esp->statics.size);
				const ImGuiWindowFlags child_flags = 0;
				const ImGuiID child_id = ImGui::GetID((void *)(intptr_t)0);
				const bool child_is_visible = ImGui::BeginChild(child_id, ImVec2(0, 600), true, child_flags);
				for (int i = 0; i < filtered->size; i++)
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
				im_record((Record *)esp->statics.elements[i]);
			ImGui::EndChild();
			ImGui::EndTabItem();
		}*/
		if (ImGui::BeginTabItem("formIds"))
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
		}
		ImGui::EndTabBar();
	}

	ImGui::End();
}