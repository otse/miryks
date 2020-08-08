#include "dark2.h"

#include "files"
#include "../opengl/group"
#include "../opengl/geometry"
#include "../opengl/camera"

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

	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize;
	ImGui::SetNextWindowSize(ImVec2(400, 0));
	ImGui::Begin(OPENGL_GUI, nullptr, flags);

	ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("tabs", tabBarFlags))
	{
		ImGui::Text((string("groups: ") + to_string(Group::num)).c_str());
		ImGui::Text((string("geometries: ") + to_string(Geometry::num)).c_str());
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