#include <opengl/group.h>
#include <opengl/drawgroup.h>
#include <opengl/geometry.h>
#include <opengl/material.h>
#include <opengl/camera.h>

#include <algorithm>

DrawGroupFlatSorted::DrawGroupFlatSorted(group_type *group, mat4 matrix)
	: drawgroup(group, matrix)
{
	Reset();
	SortTransparency();
}

void DrawGroupFlatSorted::Reset()
{
	drawgroup::Reset();
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
	for (group_type *group : target->flat)
		group->Draw(place);
	DrawBounds();
}

void DrawGroupFlatSorted::SortWith(std::function<bool(const group_type *, const group_type *)> f)
{
	std::sort(flat.begin(), flat.end(), f);
}

void DrawGroupFlatSorted::SortTransparency()
{
	// This kind of works but nobody knows
	SortWith([&](const group_type *a, const group_type *b) -> bool {
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