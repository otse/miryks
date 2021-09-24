extern "C"
{	
#include <half.h>
}

#include "trash.h"

#include <dark/dark.h>
#include <skyrim/model.h>

#include <imgui.h>

#include "../gooey/gooey.h"

#include <renderer/group.h>
#include <renderer/drawgroup.h>
#include <renderer/scene.h>
#include <renderer/shader.h>
#include <renderer/texture.h>
#include <renderer/renderer.h>
#include <renderer/rendertarget.h>

void cont_menu();

namespace skyrim
{
	SKContainer *SKContainer::cur = nullptr;

	//ItemRenderer *itemRenderer = nullptr;

	void SKContainer::Render()
	{
		cont_menu();
	}

	void SKContainer::Init()
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
			SKRecord wrcd = esp_get_form_id(cnto->item);
			if (wrcd.valid())
			{
				for (unsigned int j = 0; j < cnto->count; j++)
				{
					SKItem item(wrcd);
					items.push_back(item);
				}
			}
		}
	}
}

void cont_menu()
{
	ImGuiIO& io = ImGui::GetIO();

	io.DisplaySize.x;

	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration;
	ImGui::SetNextWindowSize(ImVec2(600, 600));
	ImGui::SetNextWindowPos(ImVec2(width - Refs::projected.x, Refs::projected.y));

	ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));

	ImGui::Begin("Container Menu", nullptr, flags);

	const SKContainer *cont = SKContainer::cur;

	auto name = cont->data<const char *>("FULL");

	ImGui::PushFont(dark::font2);
	ImGui::TextWrapped("%s with %u things", name, cont->items.size());
	ImGui::Separator();

	for (const SKItem &item : cont->items)
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
	NIF nif = get_nif(rc->path);

	static Mesh *mesh = nullptr;
	static DrawGroup *drawGroup = nullptr;
	if (mesh)
	{
		myScene->drawGroups.Remove(drawGroup);
		delete mesh;
		delete drawGroup;
	}
	mesh = new Mesh(nif);
	drawGroup = new DrawGroup(mesh->baseGroup, mat4(1.0));
	myScene->drawGroups.Add(drawGroup);
	hide_cursor();
	cameraCur = viewerCam;
	viewerCam->pos = drawGroup->aabb.center();
	//viewerCam->pos = personCam->pos;
	viewerCam->radius = drawGroup->aabb.radius2() * 2;
}
*/