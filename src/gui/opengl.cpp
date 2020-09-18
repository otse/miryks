#include <gloom/dark2.h>

#include "opengl/group.h"
#include "opengl/geometry.h"
#include "opengl/camera.h"
#include "opengl/shader.h"

//#include <glm/gtx/string_cast.hpp>
#include <sstream>
#include <imgui.h>

#define OPENGL_GUI "opengl"

using namespace gloom;

static std::stringstream ss;

#define cls           \
	ss.str(std::string()); \
	ss.clear();

void opengl_gui()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings;
	ImGui::SetNextWindowSize(ImVec2(400, 0));
	ImGui::Begin(OPENGL_GUI, nullptr, flags);

	ImGui::Text("groups: %i", Group::num);
	ImGui::Text("geometries: %i", Geometry::num);

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
			ImGui::Checkbox("axis-aligned bounding boxes", &renderSettings.axisAlignedBoundingBoxes);
			ImGui::Checkbox("oriented bounding boxes", &renderSettings.orientedBoundingBoxes);
            ImGui::SliderFloat("minimum size", &renderSettings.maximumBoundingVolume, 0.0f, 10000000.0f, "%.1f");

			if (a||b||c||d||e)
			{
				for (auto &pair : Shader::shaders)
				{
					pair.second->Compile();
				}
			}

			ImGui::Image((void*)(intptr_t)7, ImVec2(512,512));
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("camera"))
		{
			//ss << "pos: " << glm::to_string(camera->view);
			ImGui::TextWrapped(ss.str().c_str());
			cls;
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}