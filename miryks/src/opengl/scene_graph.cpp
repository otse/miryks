#include <opengl/scene_graph.hpp>

int Group::ids = 0;
int Group::num = 0;
int Group::drawCalls = 0;
int GroupDrawer::num = 0;
int GroupDrawer::masks = ~0;

Group::Group()
{
	id = ids++;
	parent = nullptr;
	geometry = nullptr;
	axis = nullptr;
	axis = new Geometry;
	axis->SetupMesh();
	matrix = matrixWorld = mat4(1.0f);
	num++;
}

Group::~Group()
{
	num--;
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

/*
Recalculates world matrices

Axis-Aligned GroupBounded overrides this
*/
void Group::UpdateSideways()
{
	if (parent == nullptr)
		matrixWorld = matrix;
	else
		matrixWorld = parent->matrixWorld * matrix;
	for (Group *child : childGroups)
		child->UpdateSideways();
}

void Group::Draw(const mat4 &left)
{
	drawCalls++;
	mat4 place = left * matrixWorld;
	// out-comment
	// if (GetZ(place) > renderSettings.drawDistance)
	//	return;
	if (geometry)
		geometry->Draw(place);
	if (axis && renderSettings.axes)
		axis->Draw(place);
}

void Group::DrawSelfAndChilds(const mat4 &left)
{
	if (!visible)
		return;
	Draw(left);
	for (Group *child : childGroups)
		child->DrawSelfAndChilds(left);
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

float Group::GetZ(const mat4 &left) const
{
	return glm::distance(cameraCur->group->matrixWorld[3], left[3]);
}

GroupBounded::GroupBounded() : Group()
{
	aabb = obb = Aabb();
	axis = new Geometry;
	axis->SetupMesh();
}

GroupBounded::~GroupBounded()
{
}

void GroupBounded::UpdateSideways()
{
	aabb = Aabb();
	Group::UpdateSideways();
	// Once everything is updated,
	// run back to front:

	if (geometry)
		aabb.extend(geometry->aabb);
	aabb = Aabb::mult(aabb, matrix);
	if (parent)
	{
		GroupBounded *bounded = dynamic_cast<GroupBounded *>(parent);
		if (bounded)
			bounded->aabb.extend(aabb);
	}
}

// special group that shouldnt trigger parenting

GroupDrawer::GroupDrawer(Group *group, mat4 matrix)
	: target(group)
{
	this->matrix = matrix;
	mask = 1 << 0;
	parent = nullptr;
	num++;
	Reset();
	UpdateSideways();
}

GroupDrawer::~GroupDrawer()
{
	num--;
}

void GroupDrawer::Reset()
{
	Cubify();
	if (target)
		TestTransparency(target);
}

void GroupDrawer::Draw(const mat4 &left)
{
	if (Invisible())
		return;
	mat4 place = matrixWorld;
	if (target)
		target->DrawSelfAndChilds(place);
	DrawBounds();
}

/*virtual void DrawSelfAndChilds() {

}*/

bool GroupDrawer::Invisible()
{
	if (visible && (GroupDrawer::masks & mask) == mask)
		return false;
	return true;
}

bool GroupDrawer::HasTransparency(Group *group)
{
	return group->geometry && group->geometry->material && group->geometry->material->transparent;
}

void GroupDrawer::TestTransparency(Group *group)
{
	hasTransparency = HasTransparency(group);
	for (auto child : group->childGroups)
		TestTransparency(child);
}

void GroupDrawer::Cubify()
{
	GroupBounded *bounded = dynamic_cast<GroupBounded *>(target);
	if (bounded)
	{
		obb = aabb = bounded->aabb;
		aabb = Aabb::mult(aabb, matrix);
		obb.geometrize();
		aabb.geometrize();
	}
}

void GroupDrawer::DrawBounds()
{
	if (!target)
		return;
	mat4 place = matrix * target->matrix;
	bool notTooLarge = aabb.volume() <= renderSettings.maximumBoundingVolume;
	if (renderSettings.AABBS && aabb.geometry && notTooLarge)
		aabb.geometry->Draw(mat4(1.0));
	if (renderSettings.OBBS && obb.geometry && notTooLarge)
		obb.geometry->Draw(place);
}

#if 0
GroupDrawerFlat::GroupDrawerFlat(Group *group, mat4 matrix)
	: GroupDrawer(group, matrix)
{
	Reset();
	SortTransparency();
}

GroupDrawerFlat::~GroupDrawerFlat()
{
}

void GroupDrawerFlat::Draw(const mat4 &left)
{
	if (target == nullptr)
		return;
	if (Invisible())
		return;
	mat4 place = matrixWorld;//matrix * target->matrix;
	for (Group *group : target->flat)
		group->Draw(place);
	DrawBounds();
}

void GroupDrawerFlat::Reset()
{
	GroupDrawer::Reset();
	if (target)
	{
		target->Flatten(target);
		// this vector copy possibly caused widespread mayhem
		//flat = target->flat; // copy?
	}
}

void GroupDrawerFlat::SortWith(std::function<bool(const Group *, const Group *)> f)
{
	std::sort(flat.begin(), flat.end(), f);
}

void GroupDrawerFlat::SortTransparency()
{
	// This kind of works but nobody knows
	SortWith([&](const Group *a, const Group *b) -> bool
			 {
		if (a->geometry && a->geometry->material && a->geometry->material->transparent)
			this->hasTransparency = true;
		if (a->geometry && b->geometry)
		{
			if (a->geometry->material->blending && !b->geometry->material->blending)
				return false;
			if (a->geometry->material->ztest && !b->geometry->material->ztest)
				return false;
			return true;
		}
		return false; });
}
#endif