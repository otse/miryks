#include <Gloom/Gloom.h>
#include <Gloom/Files.h>

extern "C"
{
#include "common.h"
#include "esp.h"
}

using namespace gloom;

#include <sstream>
#include <imgui.h>

static std::stringstream ss;

static Plugin *plugin = NULL;

void im_grup(Grup *, int);
void im_record(Record *);
void im_subrecord(Field *);

void im_grup(Grup *grup, int top_grup = -1)
{
	char t[100];
	snprintf(t, 100, "GRUP %i %s", grup->id, top_grup != -1 ? plugin->tops[top_grup] : "");
	if (ImGui::TreeNode(t))
	{
		char s[100];
		esp_print_grup(plugin, s, grup);
		ImGui::Text(s);
		for (unsigned int i = 0; i < grup->mixed.size; i++)
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

void im_record(Record *record)
{
	char *edid = nullptr;
	Field *first = record->fields.subrecords[0];
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
		for (unsigned int i = 0; i < record->fields.size; i++)
		{
			im_subrecord(record->fields.subrecords[i]);
		}
		ImGui::TreePop();
	}
}

void im_subrecord(Field *field)
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
	ImGuiIO &io = ImGui::GetIO();

	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize; // | ImGuiWindowFlags_NoSavedSettings;
	ImGui::SetNextWindowSize(ImVec2(450, io.DisplaySize.y));
	ImGui::SetNextWindowPos(ImVec2(450, 0));

	ImGui::Begin("Plugin", nullptr, flags);

#define MAX 230
	static char buf[MAX] = "Skyrim.esm";
	static char buf2[MAX] = {'\0'};

	static EspCArray *filtered = NULL;

	ImGui::InputText("##plugin", buf, IM_ARRAYSIZE(buf));

	bool reset = false;

	if (strcmp(buf, buf2))
	{
		memcpy(buf2, buf, MAX);
		// Unload gui-mounted plugin
		if (plugin && !has_plugin(plugin->name))
			free_plugin(&plugin);
		Plugin *replace = load_plugin(buf, false);
		if (replace)
		{
			plugin = replace;
			if (filtered) {
				free_esp_array(filtered);
				filtered = NULL;
			}
			reset = true;
		}
	}

	ImGui::Separator();

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
			for (unsigned int i = 0; i < plugin->grups.size; i++)
				im_grup((Grup *)plugin->grups.elements[i], i);
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
					free_esp_array(filtered);
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
					im_record((Record *)filtered->elements[i]);
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