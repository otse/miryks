#include <opengl/group.h>

#include <opengl/bound.h>
#include <opengl/geometry.h>

int Group::num = 0;

Group::Group()
{
	num++;
	parent = nullptr;
	geometry = nullptr;
	axis = nullptr;

	bound = new Bound(this);

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
	if (parent == nullptr)

		matrixWorld = matrix;

	else

		matrixWorld = parent->matrixWorld * matrix;

	for (Group *child : groups)
	
		child->Update();
	
	bound->Update();
}

void Group::DrawClassic(const mat4 &model)
{
	Draw(model);

	for (Group *child : groups)

		child->DrawClassic(model);
}

void Group::Draw(const mat4 &model)
{
	mat4 place = model * matrixWorld;

	if (axis)

		axis->Draw(place);
		
	if (geometry)

		geometry->Draw(place);
	
	if (bound)

		bound->Draw();
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