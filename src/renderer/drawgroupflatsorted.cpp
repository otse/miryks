#include <renderer/group.h>
#include <renderer/drawgroup.h>
#include <renderer/geometry.h>
#include <renderer/material.h>
#include <renderer/camera.h>

#include <algorithm>

DrawGroupFlatSorted::DrawGroupFlatSorted(Group *group, mat4 matrix)
	: DrawGroup(group, matrix)
{
	ManualReset();
	SortDefault();
}

void DrawGroupFlatSorted::ManualReset()
{
	DrawGroup::ManualReset();
	target->Flatten(target);
}

void DrawGroupFlatSorted::Draw(const mat4 &left)
{
	if (!ShouldRender())
		return;
	mat4 place = matrix * target->matrix;
	for (Group *group : target->flat)
		group->Draw(place);
	DrawBounds();
}

void DrawGroupFlatSorted::Sort(std::function<bool(const Group *, const Group *)> f)
{
	target->Flatten(target);
	auto &flat = target->flat;
	std::sort(flat.begin(), flat.end(), f);
}

void DrawGroupFlatSorted::SortDefault()
{
	Sort([&](const Group *a, const Group *b) -> bool {
		if (a->geometry && a->geometry->material && a->geometry->material->transparent)
			this->hasTransparency = true;
		if (a->geometry && b->geometry)
		{
			if (a->geometry->material->blending && !b->geometry->material->blending)
				return false;
			if (a->geometry->material->testing && !b->geometry->material->testing)
				return false;
			return true;
		}
		return false;
	});
}