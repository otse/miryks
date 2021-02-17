#ifndef OPENGL_GROUP_H
#define OPENGL_GROUP_H

#include <opengl/types.h>
#include <opengl/aabb.h>

struct Group
{
	static int Num;

	mat4 matrix, matrixWorld;

	bool visible = true;

	std::string name;

	Group *parent = nullptr;
	Geometry *geometry = nullptr;

	std::vector<Group *> groups, flat;

	Group();
	Group::~Group()
	{
		Num--;
	}
	void Add(Group *);
	void Remove(Group *);
	void DrawBegin(const mat4 &);
	void Flatten(Group *);
	virtual void Update();
protected:
	virtual void Draw(const mat4 &);
};
struct BoundedGroup : Group
{
	BoundedGroup();
	~BoundedGroup(){};
	Aabb aabb, obb;
	Geometry *axis = nullptr;
	virtual void Update();
protected:
	virtual void Draw(const mat4 &);
};

struct DrawGroup
{
	Group *const group;
	DrawGroup(Group *, mat4 = mat4(1.0));
	void Draw();
	void Reset();
	Aabb aabb, obb;
	mat4 matrix;
};

#endif