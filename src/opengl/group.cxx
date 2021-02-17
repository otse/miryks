#include <Attic.hpp>

#include <opengl/group.h>
#include <opengl/geometry.h>

int Group::Num = 0;

Group::Group()
{
	Num++;
	matrix = matrixWorld = mat4(1.0f);
}

void Group::Add(Group *group)
{
	group->parent = this;
	SafeAdd<Group *>(group, groups);
}

void Group::Remove(Group *group)
{
	group->parent = nullptr;
	SafeRemove<Group *>(group, groups);
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

void Group::DrawBegin(const mat4 &model)
{
	if (!visible)
		return;
	Draw(model);
	for (Group *child : groups)
		child->DrawBegin(model);
}

void Group::Draw(const mat4 &model)
{
	mat4 place = model * matrixWorld;
	if (geometry)
		geometry->Draw(place);
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