#if 0

#include <opengl/renderable.h>

#include <opengl/group.h>
#include <opengl/geometry.h>
#include <opengl/material.h>
#include <opengl/shader.h>

// we use drawgroups now
Renderable::Renderable(mat4 mat, Group *group) : matrix(mat), group(group)
{
	Separate();

	aabb.geometrize();
}

Renderable::~Renderable()
{
}

void Renderable::Separate()
{
	aabb = Aabb();

	objects.clear();

	group->Flatten(group);

	for (Group *group : group->flat)
	{
		//if (!group->geometry)
		//continue;

		RenderItem render_item = RenderItem(group, this);

		objects.push_back(render_item);

		aabb.extend(render_item.aabb);
	}
}

void Renderable::BeginDraws()
{
	group->BeginDraws(matrix);

	if (true)
	{
		aabb.draw(mat4(1.0));
	}
}

RenderItem::RenderItem(Group *group, Renderable *renderable) : group(group), renderable(renderable)
{
	SetInfo();

	// following is just for aabbs
	matrix = renderable->matrix * group->matrixWorld;

	if (group->geometry)
	{
		aabb = obb = group->geometry->aabb;

		aabb = Aabb::mult(aabb, matrix);

		if (true)
		{
			aabb.geometrize();
			obb.geometrize();
		}
	}
}

void RenderItem::Draw()
{
	group->Draw(renderable->matrix);

	if (renderSettings.AABBS && aabb.volume() <= renderSettings.maximumBoundingVolume)
		aabb.draw(mat4(1.0));

	if (renderSettings.OBBS && aabb.volume() <= renderSettings.maximumBoundingVolume)
		obb.draw(renderable->matrix * group->matrixWorld);
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