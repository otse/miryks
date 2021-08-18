#include <renderer/group.h>
#include <renderer/drawgroup.h>
#include <renderer/geometry.h>
#include <renderer/material.h>
#include <renderer/camera.h>

#include <algorithm>

// this lets you sort within!

DrawGroupSortable::DrawGroupSortable(Group *group, mat4 matrix)
	: DrawGroup(group, matrix)
{
	Reset();
	SortDefault();
}

void DrawGroupSortable::Reset()
{
	DrawGroup::Reset();
	group->Flatten(group);
}

void DrawGroupSortable::Draw()
{
	mat4 place = matrix * group->matrix;
	for (Group *group : group->flat)
		group->DrawSingle(place);
	DrawBounds();
}

void DrawGroupSortable::Sort(std::function<bool(const Group *, const Group *)> f)
{
	group->Flatten(group);
	auto &flat = group->flat;
	std::sort(flat.begin(), flat.end(), f);
}

void DrawGroupSortable::SortDefault()
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