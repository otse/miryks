#ifndef OGL_RENDERABLE_H
#define OGL_RENDERABLE_H

#include "renderable"

#include "group"
#include "geometry"
#include "material"
#include "shader"

// #define DRAW_AABB_AND_OBB

Renderable::Renderable(mat4 mat, Group *group) : matrix(mat), group(group)
{
	Separate();
}

Renderable::~Renderable()
{
}

void Renderable::Separate()
{
	items.clear();

	group->Flatten(group);

	for (Group *group : group->flat)
	{
		if (!group->geometry)
			continue;
		RenderItem render_item = RenderItem(group, this);
		items.push_back(render_item);
		bb.extend(render_item.bb);
	}
}

void Renderable::DrawClassic()
{
	group->DrawClassic(matrix);
}

RenderItem::RenderItem(Group *group, Renderable *renderable) : group(group), renderable(renderable)
{
	SetInfo();

	matrix = renderable->matrix * group->matrixWorld;

	obb = group->geometry->bb;

	bb = aabb::mult(obb, matrix);

#ifdef DRAW_AABB_AND_OBB

	bb.geometrize();
	obb.geometrize();

#endif

}

void RenderItem::Draw()
{
	group->Draw(renderable->matrix);

#ifdef DRAW_AABB_AND_OBB

	bb.draw(mat4(1.0));
	obb.draw(renderable->matrix);

#endif

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