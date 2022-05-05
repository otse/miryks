
#include <ogl/scene_graph.hpp>
#include <ogl/scene_graph.hpp>
#include <opengl/geometry.h>
#include <opengl/camera.h>
#include <opengl/shader.h>

#include <miryks/actors.h>

//#include <glm/gtx/string_cast.hpp>
#include <sstream>
#include <imgui.h>

static std::stringstream ss;

#define cls                \
	ss.str(std::string()); \
	ss.clear();

void overlay_renderer()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings;
	
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(450, 0));

	ImGui::Begin("Render", nullptr, flags);

	ImGui::Text("groups: %i", Group::num);
	ImGui::Text("geometries: %i", Geometry::num);

	ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("tabs", tabBarFlags))
	{
		// map to actual words
		static std::map<const char *, const char *> map = {
			{ "0", "Everything else" },
			{ References, STRINGIFY(References) },
			{ Statics, STRINGIFY(Statics) },
			{ Doors, STRINGIFY(Doors) },
			{ Furniture, STRINGIFY(Furniture) },
			{ Books, STRINGIFY(Books) },
			{ Containers, STRINGIFY(Containers) },
			{ Armor, STRINGIFY(Armor) },
			{ Weapons, STRINGIFY(Weapons) },
			{ Ammo, STRINGIFY(Ammo) },
			{ Misc, STRINGIFY(Misc) },
			{ Alchemy, STRINGIFY(Alchemy) },
			{ Ingredients, STRINGIFY(Ingredients) },
			{ Lights, STRINGIFY(Lights) },
			{ Mists, STRINGIFY(Mists) },
			{ Plants, STRINGIFY(Plants) },
			{ Flora, STRINGIFY(Flora) },
		};
		if (ImGui::BeginTabItem("objects"))
		{
			bool b = GroupDrawer::masks == 0;
			if (ImGui::Checkbox("0", &b))
				GroupDrawer::masks = 0;

			bool b2 = GroupDrawer::masks == ~0;
			if (ImGui::Checkbox("~0", &b2))
				GroupDrawer::masks = ~0;

			ImGui::Separator();
			
			int i = 0;
			for(auto word : Things) {
				int bit = 1 << i;
				bool test = (GroupDrawer::masks & bit) == bit;
				if (ImGui::Checkbox(map[word], &test))
					GroupDrawer::masks ^= bit;
				i++;
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("settings"))
		{
			ImGui::Checkbox("slow animation", &renderSettings.slowAnimation);
			bool a = ImGui::Checkbox("diffuse maps", &renderSettings.diffuseMaps);
			bool b = ImGui::Checkbox("normal maps", &renderSettings.normalMaps);
			bool c = ImGui::Checkbox("specular maps", &renderSettings.specularMaps);
			bool d = ImGui::Checkbox("glow maps", &renderSettings.glowMaps);
			bool e = ImGui::Checkbox("dust", &renderSettings.dust);
			ImGui::Checkbox("center axes", &renderSettings.axes);
			ImGui::Checkbox("axis-aligned bounding boxes", &renderSettings.AABBS);
			ImGui::Checkbox("oriented bounding boxes", &renderSettings.OBBS);
			ImGui::SliderFloat("drawDistance", &renderSettings.drawDistance, 0.0f, 10000.0f, "%.1f");
			ImGui::SliderFloat("cull big boxers", &renderSettings.maximumBoundingVolume, 0.0f, 15000000.0f, "%.1f");
			ImGui::SliderFloat("t pose to anim ratio", &renderSettings.restRatio, 0.0f, 1.0f, "%.1f");

			if (a || b || c || d || e)
			{
				for (auto &pair : Shader::shaders)
				{
					pair.second->Compile();
				}
			}

			ImVec4 color = ImVec4(sceneDef->ambient.x, sceneDef->ambient.y, sceneDef->ambient.z, 1);

			ImGuiColorEditFlags misc_flags = ImGuiColorEditFlags_None;

			ImGui::Text("Color widget:");
			ImGui::ColorEdit3("MyColor##1", (float *)&color, misc_flags);

			sceneDef->ambient = vec3(color.x, color.y, color.z);

			ImGui::Image((void *)(intptr_t)7, ImVec2(512, 512));
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("cameraCur"))
		{
			//ss << "pos: " << glm::to_string(cameraCur->view);
			ImGui::TextWrapped(ss.str().c_str());
			cls;
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("player"))
		{
			//ss << "pos: " << glm::to_string(cameraCur->view);
			//ImGui::TextWrapped("pose %s", glm::to_string(player1->pose));
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}