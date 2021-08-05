#include <dark/dark.h>
#include <dark/actor.h>

#include <renderer/group.h>
#include <renderer/geometry.h>
#include <renderer/camera.h>
#include <renderer/shader.h>

//#include <glm/gtx/string_cast.hpp>
#include <sstream>
#include <imgui.h>

using namespace dark;

static std::stringstream ss;

#define cls                \
	ss.str(std::string()); \
	ss.clear();

void opengl_gui()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings;
	ImGui::SetNextWindowSize(ImVec2(400, 0));
	ImGui::Begin("OpenGL", nullptr, flags);

	ImGui::Text("groups: %i", Group::Num);
	ImGui::Text("geometries: %i", Geometry::Num);

	ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("tabs", tabBarFlags))
	{

		if (ImGui::BeginTabItem("settings"))
		{
			bool a = ImGui::Checkbox("diffuse maps", &renderSettings.diffuseMaps);
			bool b = ImGui::Checkbox("normal maps", &renderSettings.normalMaps);
			bool c = ImGui::Checkbox("specular maps", &renderSettings.specularMaps);
			bool d = ImGui::Checkbox("glow maps", &renderSettings.glowMaps);
			bool e = ImGui::Checkbox("dust", &renderSettings.dust);
			ImGui::Checkbox("center axes", &renderSettings.axes);
			ImGui::Checkbox("axis-aligned bounding boxes", &renderSettings.AABBS);
			ImGui::Checkbox("oriented bounding boxes", &renderSettings.OBBS);
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
			ImGui::TextWrapped("pose %s", glm::to_string(player1->pose));
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}