#ifndef OGL_SCENE_GRAPH_H
#define OGL_SCENE_GRAPH_H

#include <functional>
#include <algorithm>

#include <templates.hpp>

#include <opengl/renderer.h>
#include <opengl/camera.h>
#include <opengl/aabb.h>
#include <opengl/geometry.h>
#include <opengl/material.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

struct Group
{
	static int num, drawCalls;
	bool visible = true;
	Group *parent;
	Geometry *geometry, *axis;
	mat4 matrix, matrixWorld;
	std::vector<Group *> childGroups, flat;
	Group()
	{
		parent = nullptr;
		geometry = axis = nullptr;
		axis = new Geometry;
		axis->SetupMesh();
		matrix = matrixWorld = mat4(1.0f);
		num++;
	}
	
	virtual ~Group()
	{
		num--;
	}

	void Add(Group *group)
	{
		group->parent = this;
		vector_safe_add<Group *>(group, childGroups);
	}

	void Remove(Group *group)
	{
		group->parent = nullptr;
		vector_safe_remove<Group *>(group, childGroups);
	}

	/*
	Recalculates the scene graph 
	*/
	virtual void UpdateSideways()
	{
		if (parent == nullptr)
			matrixWorld = matrix;
		else
			matrixWorld = parent->matrixWorld * matrix;
		for (Group *child : childGroups)
			child->UpdateSideways();
	}

	virtual void Draw(const mat4 &left)
	{
		drawCalls++;
		mat4 place = left * matrixWorld;
		// out-comment
		// if (GetZ(place) > renderSettings.drawDistance)
		//	return;
		if (geometry)
			geometry->Draw(place);
		if (axis && renderSettings.axes)
			axis->Draw(place);
	}

	void DrawChilds(const mat4 &left)
	{
		if (!visible)
			return;
		Draw(left);
		for (Group *child : childGroups)
			child->DrawChilds(left);
	}

	void Flatten(Group *root)
	{
		// Put all childs into root.flat
		if (this == root)
			flat.clear();
		root->flat.push_back(this);
		for (Group *child : childGroups)
			child->Flatten(root);
	}

	float GetZ(const mat4 &left) const
	{
		return glm::distance(cameraCur->group->matrixWorld[3], left[3]);
	}
};

struct GroupBounded : Group
{
	AABB aabb, obb;
	GroupBounded() : Group()
	{
		aabb = obb = Aabb();
		axis = new Geometry;
		axis->SetupMesh();
	}

	virtual ~GroupBounded()
	{
	}

	virtual void UpdateSideways() override
	{
		aabb = Aabb();
		Group::UpdateSideways();
		// Once everything is updated,
		// run back to front:

		if (geometry)
			aabb.extend(geometry->aabb);
		aabb = Aabb::mult(aabb, matrix);
		if (parent)
		{
			GroupBounded *bounded = dynamic_cast<GroupBounded *>(parent);
			if (bounded)
				bounded->aabb.extend(aabb);
		}
	}
};

// special group that shouldnt trigger parenting

struct DrawGroup : Group
{
	static int num, masks;
	int mask;
	AABB aabb, obb;
	Group *const target;
	DrawGroup(Group *group, mat4 matrix)
		: target(group)
	{
		this->matrix = matrix;
		mask = 1 << 0;
		parent = nullptr;
		num++;
		Reset();
		UpdateSideways();
	}

	virtual ~DrawGroup()
	{
		num--;
	}

	virtual void Reset()
	{
		Cubify();
	}

	virtual void Draw(const mat4 &left) override
	{
		if (Invisible())
			return;
		mat4 right = left * matrixWorld;
		if (childGroups.size())
			for (Group *child : childGroups)
				if (dynamic_cast<DrawGroup *>(child))
					child->Draw(right);
		if (target)
			target->DrawChilds(right);
		DrawBounds();
	}

	bool Invisible()
	{
		if (visible && (DrawGroup::masks & mask) == mask)
			return false;
		return true;
	}

	void Cubify()
	{
		GroupBounded *bounded = dynamic_cast<GroupBounded *>(target);
		if (bounded)
		{
			obb = aabb = bounded->aabb;
			aabb = Aabb::mult(aabb, matrix);
			obb.geometrize();
			aabb.geometrize();
		}
	}

	void DrawBounds()
	{
		if (!target)
			return;
		mat4 place = matrix * target->matrix;
		bool notTooLarge = aabb.volume() <= renderSettings.maximumBoundingVolume;
		if (renderSettings.AABBS && aabb.geometry && notTooLarge)
			aabb.geometry->Draw(mat4(1.0));
		if (renderSettings.OBBS && obb.geometry && notTooLarge)
			obb.geometry->Draw(place);
	}
};

struct DrawGroupFlatSorted : DrawGroup
{
	bool hasTransparency = false;
	DrawGroupFlatSorted(Group *group, mat4 matrix)
		: DrawGroup(group, matrix)
	{
		Reset();
		SortTransparency();
	}

	virtual ~DrawGroupFlatSorted()
	{
	}

	virtual void Draw(const mat4 &left) override
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

	virtual void Reset() override
	{
		DrawGroup::Reset();
		if (target)
		{
			target->Flatten(target);
			flat = target->flat;
		}
	}

	void SortWith(std::function<bool(const Group *, const Group *)> f)
	{
		std::sort(flat.begin(), flat.end(), f);
	}

	void SortTransparency()
	{
		// This kind of works but nobody knows
		SortWith([&](const Group *a, const Group *b) -> bool
				 {
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
		return false; });
	}
};

#endif