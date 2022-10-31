#ifndef OGL_SCENE_GRAPH_H
#define OGL_SCENE_GRAPH_H

#include <functional>
#include <algorithm>

#include <templates.hpp>

#include <opengl/renderer.h>
#include <opengl/camera.h>
#include <opengl/aabb.h>
#include <opengl/geometry.h>
#include <opengl/material.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

struct orientation
{
	orientation();
	void Compose();
	vec3 rotation, position, scale;
	mat4 matrix;
};

struct Group
{
	static int ids, num, drawCalls;
	int id = 0;
	bool visible = true;
	bool collision = true;
	std::string name;
	Group *parent;
	Geometry *geometry, *axis;
	mat4 matrix, matrixWorld;
	std::vector<Group *> childGroups, flat;

	Group();
	virtual ~Group();

	void Add(Group *group);
	void Remove(Group *group);

	virtual void UpdateSideways();
	virtual void Draw(const mat4 &left);
	virtual void DrawSelfAndChilds(const mat4 &left) final;
	void Flatten(Group *root);
	float GetZ(const mat4 &left) const;
};

struct GroupBounded : Group
{
	AABB aabb, obb;
	GroupBounded();
	virtual ~GroupBounded();

	virtual void UpdateSideways() override;
};

// special group that shouldnt trigger parenting

struct GroupDrawer : Group
{
	bool hasTransparency = false;

	static int num, masks;
	int mask;
	// float scale is a quick collision-matrix fix
	//orientation orientation;
	AABB aabb, obb;
	Group *const target;

	GroupDrawer(Group *group, mat4 matrix);
	virtual ~GroupDrawer();

	virtual void Reset();
	virtual void Draw(const mat4 &left) override;
	bool Invisible();
	void Cubify();
	static bool HasTransparency(Group *);
	void TestTransparency(Group *);
	void DrawBounds();
};

struct GroupDrawerFlat : GroupDrawer
{
	bool hasTransparency = false;
	
	GroupDrawerFlat(Group *group, mat4 matrix);
	virtual ~GroupDrawerFlat();

	virtual void Draw(const mat4 &left) override;
	virtual void Reset() override;
	void SortWith(std::function<bool(const Group *, const Group *)> f);
	void SortTransparency();
};

#endif