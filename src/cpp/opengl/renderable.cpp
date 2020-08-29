#ifndef OGL_RENDERABLE_H
#define OGL_RENDERABLE_H

#include "renderable"

#include "group"
#include "geometry"
#include "material"
#include "shader"

const bool DRAW_BOUNDS = true;

Renderable::Renderable(mat4 mat, Group *group) : matrix(mat), group(group)
{
	aabb = AABB(0);

	Separate();

	aabb.geometrize();
}

Renderable::~Renderable()
{
}

void Renderable::Separate()
{
	objects.clear();

	group->Flatten(group);

	for (Group *group : group->flat)
	{
		if (!group->geometry)
			continue;

		RenderItem render_item = RenderItem(group, this);

		objects.push_back(render_item);

		aabb.extend(render_item.obb);
	}
}

void Renderable::DrawClassic()
{
	group->DrawClassic(matrix);

	//obb.draw(mat4(1.0));
}

RenderItem::RenderItem(Group *group, Renderable *renderable) : group(group), renderable(renderable)
{
	SetInfo();

	matrix = renderable->matrix * group->matrixWorld;

	aabb = obb = group->geometry->aabb;

	aabb = AABB::mult(aabb, matrix);

	if (DRAW_BOUNDS)
	{
		aabb.geometrize();
		obb.geometrize();
	}
}

void RenderItem::Draw()
{
	group->Draw(renderable->matrix);

	if (DRAW_BOUNDS)
	{
		aabb.draw(mat4(1.0));
		obb.draw(renderable->matrix);
	}
}

void RenderItem::TransformVertices()
{
	// Used for collision detection

	if (triangles.size() > 0)
		return;

	for (size_t i = 0, j = group->geometry->elements.size(); i < j; i += 3)
	{
		auto a = group->geometry->elements[i + 0];
		auto b = group->geometry->elements[i + 1];
		auto c = group->geometry->elements[i + 2];

		Triangle tr = {
			vec3(matrix * vec4(group->geometry->vertices[a].position, 1)),
			vec3(matrix * vec4(group->geometry->vertices[b].position, 1)),
			vec3(matrix * vec4(group->geometry->vertices[c].position, 1))};

		tr.n = glm::normalize(glm::cross(tr.y - tr.x, tr.x - tr.z));

		triangles.push_back(tr);
	}
}

void RenderItem::SetInfo()
{
	if (group->geometry &&
		group->geometry->material)
		material = group->geometry->material;

	if (material &&
		material->shader)
		program = material->shader->id;

	if (material && material->transparent)
		score += 1;
	if (material && material->testing)
		score += 1;
}

#endif