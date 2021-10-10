#include <renderer/group.h>
#include <renderer/drawgroup.h>
#include <renderer/geometry.h>
#include <renderer/material.h>
#include <renderer/camera.h>

#include <algorithm>

DrawGroupFlatSorted::DrawGroupFlatSorted(Group *group, mat4 matrix)
	: DrawGroup(group, matrix)
{
	Reset();
	SortTransparency();
}

void DrawGroupFlatSorted::Reset()
{
	DrawGroup::Reset();
	if (target)
	{
		target->Flatten(target);
		flat = target->flat;
	}
}

void DrawGroupFlatSorted::Draw(const mat4 &left)
{
	if (target == nullptr)
		return;
	if (Invisible())
		return;
	mat4 place = matrix * target->matrix;
	for (Group *group : target->flat)
		group->Draw(place);
	DrawBounds();
}

void DrawGroupFlatSorted::SortWith(std::function<bool(const Group *, const Group *)> f)
{
	std::sort(flat.begin(), flat.end(), f);
}

void DrawGroupFlatSorted::SortTransparency()
{
	// This kind of works but nobody knows
	SortWith([&](const Group *a, const Group *b) -> bool {
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