#include <opengl/group.h>

#include <opengl/geometry.h>

int Group::num = 0;

Group::Group()
{
	num++;
	parent = nullptr;
	geometry = nullptr;
	axis = nullptr;

	matrix = mat4(1.0f);
	matrixWorld = mat4(1.0f);

	axis = new Geometry;
	axis->SetupMesh();
}

Group::~Group()
{
	num--;
}

void Group::Add(Group *group)
{
	group->parent = this;

	groups.push_back(group);
}

void Group::Update()
{
	aabb = AABB();

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

	aabb = AABB::mult(aabb, matrix);
}

void Group::DrawBegin(const mat4 &model)
{
	Draw(model);

	for (Group *child : groups)

		child->DrawBegin(model);
}

void Group::Draw(const mat4 &model)
{
	mat4 place = model * matrixWorld;

	if (axis)

		axis->Draw(place);

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

DrawGroup::DrawGroup(Group *group, mat4 matrix) : group(group), matrix(matrix)
{
	Reset();
}

void DrawGroup::Reset()
{
	obb = aabb = group->aabb;

	aabb = AABB::mult(aabb, matrix);

	aabb.geometrize();

	obb.geometrize();
}

void DrawGroup::Draw()
{
	group->DrawBegin(matrix);

	mat4 place = matrix * group->matrixWorld;

	if (renderSettings.AABBS && aabb.volume() <= renderSettings.maximumBoundingVolume)
	{
		aabb.geometry->Draw(mat4(1.0));
	}

	if (renderSettings.OBBS && aabb.volume() <= renderSettings.maximumBoundingVolume)

		obb.geometry->Draw(place);
}