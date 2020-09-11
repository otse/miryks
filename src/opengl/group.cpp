#include "group.h"

#include "geometry.h"

const bool DRAW_AXIS = 0;

int Group::num = 0;

Group::Group()
{
	num++;
	parent = nullptr;
	geometry = nullptr;
	axis = nullptr;

	matrix = mat4(1.0f);
	matrixWorld = mat4(1.0f);

	if (DRAW_AXIS)
	{
		axis = new Geometry;
		axis->SetupMesh();
	}

}

Group::~Group() {
	num--;
}

void Group::Add(Group *group)
{
	group->parent = this;
	
	group->Update();

	groups.push_back(group);
}

void Group::Update()
{
	// Accumulate matrices

	// https://github.com/mrdoob/three.js/blob/master/src/core/Object3D.js#L561
	
	if (parent == nullptr)

		matrixWorld = matrix;

	else

		matrixWorld = parent->matrixWorld * matrix;

	for (Group *group : groups)

		group->Update();
}

void Group::Draw(const mat4 &model)
{
	mat4 place = model * matrixWorld;

	if (DRAW_AXIS)

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

	for (Group *group : groups)

		group->Flatten(root);
}

void Group::DrawClassic(const mat4 &model)
{
	Draw(model);

	for (Group *group : groups)

		group->DrawClassic(model);
}

