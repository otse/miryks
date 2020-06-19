#include "group"

#include "geometry"

//#define DRAW_AXIS

Group_t::Group_t()
{
	parent = nullptr;
	geometry = nullptr;

	matrix = mat4(1.0f);
	matrix_world = mat4(1.0f);

#ifdef DRAW_AXIS

	axis = new Geometry;
	axis->SetupMesh();

#endif
}

void Group_t::Add(Group_t *gr)
{
	gr->parent = this;
	gr->Update();

	groups.push_back(gr);
}

void Group_t::Update()
{
	// Accumulate matrices

	// https://github.com/mrdoob/three.js/blob/master/src/core/Object3D.js#L561

	if (parent == nullptr)
		matrix_world = matrix;
	else
		matrix_world = parent->matrix_world * matrix;

	for (Group_t *gr : groups)
	{
		gr->Update();
	}
}

void Group_t::Draw(const mat4 &model)
{
	mat4 matrix = model * matrix_world;

#ifdef DRAW_AXIS

	axis->Draw(matrix);

#endif

	if (geometry)
		geometry->Draw(matrix);
}

void Group_t::Flatten(Group_t *root)
{
	// Put all childs into root.flat

	if (this == root)
		flat.clear();
	root->flat.push_back(this);

	for (Group_t *gr : groups)
	
		gr->Flatten(root);
}

void Group_t::DrawClassic(const mat4 &model)
{
	Draw(model);

	for (Group_t *gr : groups)
		gr->DrawClassic(model);
}