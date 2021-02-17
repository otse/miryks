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
	groups.push_back(group);
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

BoundedGroup::BoundedGroup() : Group()
{
	axis = new Geometry;
	axis->SetupMesh();
}

void BoundedGroup::Draw(const mat4 &model)
{
	Group::Draw(model);
	mat4 place = model * matrixWorld;
	if (axis && renderSettings.axes)
		axis->Draw(place);
}

/*
void Group::Update()
{
	aabb = Aabb();
	if (geometry)
		aabb.extend(geometry->aabb);
	if (parent == nullptr)
		matrixWorld = matrix;
	else
		matrixWorld = parent->matrixWorld * matrix;
	for (Group *child : groups)
	{
		child->Update();
		aabb.extend(child->aabb);
	}
	obb = aabb;
	aabb = Aabb::mult(aabb, matrix);
}
*/

void BoundedGroup::Update()
{
	Group::Update();
	aabb = Aabb();
	if (geometry)
		aabb.extend(geometry->aabb);
	for (Group *child : groups)
	{
		BoundedGroup *bounded = dynamic_cast<BoundedGroup *>(child);
		if (bounded)
			aabb.extend(bounded->aabb);
	}
	obb = aabb;
	aabb = Aabb::mult(aabb, matrix);
}

DrawGroup::DrawGroup(Group *group, mat4 matrix) : group(group), matrix(matrix)
{
	Reset();
}

void DrawGroup::Reset()
{
	//group->Update();
	BoundedGroup *bounded = dynamic_cast<BoundedGroup *>(group);
	if (bounded)
	{
		obb = aabb = bounded->aabb;
	}
	aabb = Aabb::mult(aabb, matrix);
	aabb.geometrize();
	obb.geometrize();
	//group->Flatten(group);
}

void DrawGroup::Draw()
{
	group->DrawBegin(matrix);
	mat4 place = matrix * group->matrix;
	if (renderSettings.AABBS && aabb.volume() <= renderSettings.maximumBoundingVolume)
		aabb.geometry->Draw(mat4(1.0));
	if (renderSettings.OBBS && aabb.volume() <= renderSettings.maximumBoundingVolume)
		obb.geometry->Draw(place);
}