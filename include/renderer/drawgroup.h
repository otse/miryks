#ifndef OPENGL_DRAWGROUP_H
#define OPENGL_DRAWGROUP_H

#include <functional>

#include <renderer/types.h>

#include <renderer/Aabb.h>

struct DrawGroup
{
	static int Num;
	bool toggle;
	mat4 matrix;
	DrawGroup *parent;
	Group *const target;
	std::vector<DrawGroup *> drawGroups;
	Aabb aabb, obb;
	DrawGroup(Group *, mat4);
	virtual ~DrawGroup();
	void Add(DrawGroup *);
	void Remove(DrawGroup *);
	void DrawBounds();
	virtual void Draw();
	virtual void Reset();
	void Rebound();
	float GetZ() const;
};

struct DrawGroupSortable : DrawGroup
{
	bool hasTransparency = false;;
	DrawGroupSortable(Group *, mat4);
	virtual ~DrawGroupSortable(){};
	virtual void Draw();
	virtual void Reset();
	void Sort(std::function<bool(const Group *, const Group *)>);
	void SortDefault();
};

#endif