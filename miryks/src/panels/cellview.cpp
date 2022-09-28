#include <miryks/miryks.hpp>

#include <opengl/scene.h>

#include <files.h>
#include <imgui.h>
#include <sstream>

void layout_group(Group *, int &);

void layout_gdf(Group *child, int &num)
{
	auto cast = dynamic_cast<GroupDrawer *>(child);
	char hex[50];
	snprintf(hex, 50, "GroupDrawer #%i %s", child->id, child->name.c_str());
	if (ImGui::TreeNode(hex))
	{
		ImGui::Text("target:");
		// if (dynamic_cast<GroupDrawer *>(child))

		layout_group(cast->target, num);

		ImGui::TreePop();
	}
}

void layout_group(Group *child, int &num)
{
	char hex[50];
	snprintf(hex, 50, "group #%i (childs - %i) %s", child->id, child->childGroups.size(), child->name.c_str());

	if (ImGui::TreeNode(hex))
	{
		for (auto inner : child->childGroups)
			layout_group(inner, num);
		ImGui::TreePop();
	}
};

void overlay_cellview()
{
	using namespace miryks;

	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize; // | ImGuiWindowFlags_NoSavedSettings;
	ImGui::SetNextWindowSize(ImVec2(450, 500));
	ImGui::SetNextWindowPos(ImVec2(450 * 4, 0));

	ImGui::Begin("View", nullptr, flags);

	ImGui::Separator();

	ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("CellViewTabs", tabBarFlags))
	{
		if (ImGui::BeginTabItem("scene graph"))
		{
			ImGui::Text("showing descending scene bigGroup");

			auto vec = ImVec2(0, ImGui::GetContentRegionAvail().y);
			const ImGuiWindowFlags child_flags = 0;
			const bool child_is_visible = ImGui::BeginChild("scenegraph", vec, true, child_flags);

			int num = 0;
			for (auto child : sceneDef->bigGroup->childGroups)
			{
				if (dynamic_cast<GroupDrawer *>(child))
				{
					layout_gdf(child, num);
				}
				/*else if (dynamic_cast<GroupDrawer *>(child))
				{
					layout_gd(child, num);
				}
				else {
					layout_group(child, num);
				}*/
			}
			ImGui::EndChild();

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("references"))
		{

			ImGui::Text("showing all references");

			ImGui::Separator();

			// auto vec = ImVec2(0, ImGui::GetContentRegionAvail().y);
			auto vec = ImVec2(0, ImGui::GetContentRegionAvail().y);

			const ImGuiWindowFlags child_flags = 0;
			const bool child_is_visible = ImGui::BeginChild("all references", vec, true, child_flags);

			int num = 0;
			for (auto ref : ginterior->refers)
			{
				num++;
				char hex[40];
				snprintf(hex, 40, "reference %i - %s", num, ref->baseObject);
				if (ImGui::TreeNode(hex))
				{
					ImGui::Text("whats it do %i", num);
					ImGui::TreePop();
				}
			}
			ImGui::EndChild();

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("interiors"))
		{
			static const char *items[2] = {
				"GloomGen",
				"GloomAfterGen"};

			static int num = 2;
			static int current = 0;

			ImGui::ListBox("##interiors", &current, items, num, 4);

			ImGui::Separator();

			if (!ginterior || ginterior && ginterior->editor_id(items[current]) == false)
			{
				if (ImGui::Button("Load"))
				{
					hooks::hooks_load_interior(items[current], true);
				}
			}
			// if (items[current] == ginterior->loadedCell)
			{
			}

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("world space"))
		{
			if (gworldspace)
			{
				ImGui::Text("num total exterior cells: %i", gworldspace->exteriors.size());
			}
			if (ImGui::Button("Go outdoors?"))
			{
				if (ginterior)
				{
					delete ginterior;
					ginterior = nullptr;
				}
				if (!gworldspace)
				{
					/// load_world_space("DarkWorld");
				}
			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}