#include "dark2.h"

#include "opengl/group.h"
#include "opengl/geometry.h"
#include "opengl/camera.h"
#include "opengl/shader.h"

#include <glm/gtx/string_cast.hpp>
#include <sstream>
#include <imgui.h>

#define OPENGL_GUI "opengl"

using namespace dark2;

static stringstream ss;

#define cls           \
	ss.str(string()); \
	ss.clear();

void opengl_gui()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings;
	ImGui::SetNextWindowSize(ImVec2(400, 0));
	ImGui::Begin(OPENGL_GUI, nullptr, flags);

	ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("tabs", tabBarFlags))
	{
		ImGui::Text((string("groups: ") + to_string(Group::num)).c_str());
		ImGui::Text((string("geometries: ") + to_string(Geometry::num)).c_str());
		if (ImGui::BeginTabItem("settings"))
		{
			bool a = ImGui::Checkbox("diffuse maps", &commonSettings.diffuseMaps);
			bool b = ImGui::Checkbox("normal maps", &commonSettings.normalMaps);
			bool c = ImGui::Checkbox("specular maps", &commonSettings.specularMaps);
			bool d = ImGui::Checkbox("dust", &commonSettings.dust);

			if (a||b||c||d)
			{
				for (auto &pair : Shader::shaders)
				{
					pair.second->Compile();
				}
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("camera"))
		{
			ss << "pos: " << glm::to_string(camera->view);
			ImGui::TextWrapped(ss.str().c_str());
			cls;
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}