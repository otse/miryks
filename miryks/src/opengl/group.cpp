#include <templates.hpp>

#include <opengl/camera.h>
#include <opengl/group.h>
#include <opengl/geometry.h>

int group_type::num = 0;
int group_type::drawCalls = 0;

group_type::group_type()
{
	visible = true;
	parent = nullptr;
	geometry = nullptr;
	axis = nullptr;
	matrix = matrixWorld = mat4(1.0f);
	num++;
}

group_type::~group_type()
{
	num--;
}

void group_type::Add(group_type *group)
{
	group->parent = this;
	vector_safe_add<group_type *>(group, childGroups);
}

void group_type::Remove(group_type *group)
{
	group->parent = nullptr;
	vector_safe_remove<group_type *>(group, childGroups);
}

void group_type::Update()
{
	if (parent == nullptr)
		matrixWorld = matrix;
	else
		matrixWorld = parent->matrixWorld * matrix;
	for (group_type *child : childGroups)
		child->Update();
}

void group_type::Draw(const mat4 &left)
{
	drawCalls++;
	mat4 place = left * matrixWorld;
	// out-comment
	//if (GetZ(place) > renderSettings.drawDistance)
	//	return;
	if (geometry)
		geometry->Draw(place);
	if (axis && renderSettings.axes)
		axis->Draw(place);
}

void group_type::DrawChilds(const mat4 &left)
{
	if (!visible)
		return;
	Draw(left);
	for (group_type *child : childGroups)
		child->DrawChilds(left);
}

void group_type::Flatten(group_type *root)
{
	// Put all childs into root.flat
	if (this == root)
		flat.clear();
	root->flat.push_back(this);
	for (group_type *child : childGroups)
		child->Flatten(root);
}

float group_type::GetZ(const mat4 &left) const
{
	return glm::distance(cameraCur->group->matrixWorld[3], left[3]);
}