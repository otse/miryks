#ifndef OGL_AABB_H
#define OGL_AABB_H

#include <opengl/types.h>

struct AABB
{
	enum INTERSECTION
	{
		OUTSIDE,
		INSIDE,
		INTERSECT
	};

	INTERSECTION intersect(const AABB &) const;

	static AABB mult(const AABB &, const mat4 &);

	AABB(float = 0);
	~AABB();
	vec3 min, max;

	inline bool is_zero()
	{
		return min == vec3(0) && max == vec3(0);
	};

	void extend(const float);
	void extend(const vec3 &);
	void extend(const AABB &);
	void translate(const vec3 &);

	vec3 diagional() const;
	vec3 center() const;

	bool contains_vec(const vec3 &v) const;
	float radius2() const;

	void geometrize();
	void draw(const mat4 &);
	
	Geometry *geometry = nullptr;

};

#endif