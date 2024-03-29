#if 0
extern "C"
{	
#include <half.h>
}

#include <miryks/model.h>
#include <dark/trash.h>

#include <imgui.h>

#include <panels.h>

#include <opengl/scene_graph.hpp>
#include <opengl/scene.h>
#include <opengl/shader.h>
#include <opengl/texture.h>
#include <opengl/renderer.h>
#include <opengl/rendertarget.h>

void cont_menu();

namespace miryks
{
	Container *Container::cur = nullptr;

	//ItemRenderer *itemRenderer = nullptr;

	void Container::Render()
	{
		cont_menu();
	}

	void Container::Init()
	{
		unsigned int *coct = data<unsigned int *>("COCT");

		for (unsigned int i = 0; i < *coct; i++)
		{
			// container object
			struct Cnto
			{
				unsigned int item, count;
			};
			Cnto *cnto = data<Cnto *>("CNTO", i);
			record record = esp_get_form_id(cnto->item);
			if (record.rvalid())
			{
				for (unsigned int j = 0; j < cnto->count; j++)
				{
					SKItem item(record);
					items.push_back(item);
				}
			}
		}
	}
}

void cont_menu()
{
	using namespace miryks;
	ImGuiIO& io = ImGui::GetIO();

	io.DisplaySize.x;

	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration;
	ImGui::SetNextWindowSize(ImVec2(600, 600));
	ImGui::SetNextWindowPos(ImVec2(width - itemfinder::projected.x, itemfinder::projected.y));

	ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));

	ImGui::Begin("Container Menu", nullptr, flags);

	Container *cont = Container::cur;

	auto name = cont->data<const char *>("FULL");

	ImGui::PushFont(font2);
	ImGui::TextWrapped("%s with %u things", name, cont->items.size());
	ImGui::Separator();

	for (SKItem &item : cont->items)
	{
		auto FULL = item.data<const char *>("FULL");
		auto EDID = item.data<const char *>("EDID");
		auto name = FULL ? FULL : EDID;
		ImGui::Text("%s", name);
	}

	ImGui::PopFont();

	ImGui::PopStyleColor(4);
	
	ImGui::End();
}

/*
ItemRenderer::ItemRenderer()
{
	myScene = new Scene;
	myCam = new ViewerCamera;
	myScene->ambient = vec3(100.f / 255.f);
}

void ItemRenderer::View(Item *item) {
	using namespace dark;
	this->item = item;
	auto modl = item->data<const char *>("MODL");
	if (!modl)
		return;
	printf("itemRenderer view %s", item->data<unsigned char *>("FULL"));

	RES rc = bsa_find_more(modl, 0x1);
	NIF ni = get_ni(rc->path);

	static Mesh *mesh = nullptr;
	static GroupDrawer *groupDrawer = nullptr;
	if (mesh)
	{
		myScene->drawGroups.Remove(groupDrawer);
		delete mesh;
		delete groupDrawer;
	}
	mesh = new Mesh(ni);
	groupDrawer = new GroupDrawer(mesh->baseGroup, mat4(1.0));
	myScene->drawGroups.Add(groupDrawer);
	hideCursor();
	cameraCur = viewerCam;
	viewerCam->pos = groupDrawer->aabb.center();
	//viewerCam->pos = personCam->pos;
	viewerCam->radius = groupDrawer->aabb.radius2() * 2;
}
*/
#endif