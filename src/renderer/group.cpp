#include <templates.hpp>

#include <renderer/camera.h>
#include <renderer/group.h>
#include <renderer/geometry.h>

int Group::Num = 0;

Group::Group()
{
	visible = true;
	parent = nullptr;
	geometry = nullptr;
	axis = nullptr;
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
	if (axis && renderSettings.axes)
		axis->Draw(place);
}

void Group::DrawChilds(const mat4 &left)
{
	if (!visible)
		return;
	Draw(left);
	for (Group *child : childGroups)
		child->DrawChilds(left);
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

float Group::GetZ() const
{
	return -vec3((cameraCur->view * matrix)[3]).z;
}