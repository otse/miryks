#include "group"

#include "geometry"

//#define DRAW_AXIS

group_t::group_t()
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

void group_t::add(group_t *gr)
{
	gr->parent = this;
	gr->update();

	groups.push_back(gr);
}

void group_t::update()
{
	// Accumulate matrices

	// https://github.com/mrdoob/three.js/blob/master/src/core/Object3D.js#L561

	if (parent == nullptr)
		matrix_world = matrix;
	else
		matrix_world = parent->matrix_world * matrix;

	for (group_t *gr : groups)
	{
		gr->update();
	}
}

void group_t::draw(const mat4 &model)
{
	mat4 matrix = model * matrix_world;

#ifdef DRAW_AXIS

	axis->Draw(matrix);

#endif

	if (geometry)
		geometry->Draw(matrix);
}

void group_t::flatten(group_t *root)
{
	// Put all childs into root.flat

	if (this == root)
		flat.clear();
	root->flat.push_back(this);

	for (group_t *gr : groups)
	{
		gr->flatten(root);
	}
}

void group_t::draw_classic(const mat4 &model)
{
	mat4 matrix = model * matrix_world;

	//axis->Draw(matrix);

	if (geometry)
		geometry->Draw(matrix);

	for (group_t *gr : groups)
	{
		gr->draw_classic(model);
	}
}