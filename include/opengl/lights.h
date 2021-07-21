#ifndef OPENGL_LIGHTS_H
#define OPENGL_LIGHTS_H

// part of gloom

#include <opengl/Types.h>

#include <opengl/Camera.h>
#include <opengl/Group.h>

struct Light
{
	typedef LightShadow Shadow;
	Light();
	Shadow *shadow;
	mat4 matrix;
	vec3 color;
	float intensity, distance, decay;
	float CalcDist() const;
	void Calc();
	
	vec3 target;
};
struct PointLight : Light
{
	PointLight();
};
struct SpotLight : Light
{
	SpotLight();
	float angle, penumbra;
	vec3 target;
};
struct DirectionalLight : Light
{
	DirectionalLight();
	vec3 target;
};

#endif