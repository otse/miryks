#ifndef OGL_AABB_H
#define OGL_AABB_H

// part of gloom

#include <opengl/Types.h>

struct Aabb
{
	enum INTERSECTION
	{
		OUTSIDE,
		INSIDE,
		INTERSECT
	};

	INTERSECTION intersect(const Aabb &) const;

	static Aabb mult(const Aabb &, const mat4 &);

	Aabb();
	~Aabb();
	vec3 min, max;
	bool valid;

	//void extend(const float);
	void extend(const Aabb &);
	void extend(const vec3 &);
	void translate(const vec3 &);

	vec3 diagional() const;
	vec3 center() const;

	bool contains_vec(const vec3 &v) const;
	float radius2() const;
	float volume() const;
	
	void geometrize();
	
	Geometry *geometry = nullptr;

};

#endif