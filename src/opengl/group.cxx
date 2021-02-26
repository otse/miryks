#include <Attic.hpp>

#include <OpenGL/Group.h>
#include <OpenGL/Geometry.h>

int Group::Num = 0;

Group::Group()
{
	Num++;
	matrix = matrixWorld = mat4(1.0f);
}

Group::~Group()
{
	Num--;
}

void Group::Add(Group *group)
{
	group->parent = this;
	VectorAdd<Group *>(group, groups);
}

void Group::Remove(Group *group)
{
	group->parent = nullptr;
	VectorRemove<Group *>(group, groups);
}

void Group::Update()
{
	if (parent == nullptr)
		matrixWorld = matrix;
	else
		matrixWorld = parent->matrixWorld * matrix;
	for (Group *child : groups)
		child->Update();
}

void Group::DrawSingle(const mat4 &model)
{
	mat4 place = model * matrixWorld;
	if (geometry)
		geometry->Draw(place);
}

void Group::DrawMultiple(const mat4 &model)
{
	if (!visible)
		return;
	DrawSingle(model);
	for (Group *child : groups)
		child->DrawMultiple(model);
}

void Group::Flatten(Group *root)
{
	// Put all childs into root.flat
	if (this == root)
		flat.clear();
	root->flat.push_back(this);
	for (Group *child : groups)
		child->Flatten(root);
}