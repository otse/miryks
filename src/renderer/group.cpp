#include <templates.hpp>

#include <renderer/group.h>
#include <renderer/geometry.h>

int Group::Num = 0;

Group::Group()
{
	visible = true;
	parent = nullptr;
	geometry = nullptr;
	matrix = matrixWorld = mat4(1.0f);
	Num++;
}

Group::~Group()
{
	Num--;
}

void Group::Add(Group *group)
{
	group->parent = this;
	vector_safe_add<Group *>(group, childGroups);
}

void Group::Remove(Group *group)
{
	group->parent = nullptr;
	vector_safe_remove<Group *>(group, childGroups);
}

void Group::Update()
{
	if (parent == nullptr)
		matrixWorld = matrix;
	else
		matrixWorld = parent->matrixWorld * matrix;
	for (Group *child : childGroups)
		child->Update();
}

void Group::Draw(const mat4 &left)
{
	mat4 place = left * matrixWorld;
	if (geometry)
		geometry->Draw(place);
}

void Group::DrawRecursive(const mat4 &left)
{
	if (!visible)
		return;
	Draw(left);
	for (Group *child : childGroups)
		child->DrawRecursive(left);
}

void Group::Flatten(Group *root)
{
	// Put all childs into root.flat
	if (this == root)
		flat.clear();
	root->flat.push_back(this);
	for (Group *child : childGroups)
		child->Flatten(root);
}