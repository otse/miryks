#ifndef OPENGL_CAMERA_H
#define OPENGL_CAMERA_H

// part of gloom

#include <renderer/renderer.h>

#include <glm/gtc/matrix_transform.hpp>

struct Camera
{
public:
	float yaw, pitch, fov;
	Camera();

	Group *group;
	DrawGroup *drawGroup;

	vec3 pos;
	mat4 view, /*matrix,*/ projection;

	bool disabled = false;

	virtual void Mouse(float, float){};
	virtual void Update(float){};

	void SetProjection();
};

struct FirstPersonCamera : public Camera
{
	Group *hands;

	vec3 eye;

	FirstPersonCamera();

	virtual void Mouse(float, float);
	virtual void Update(float);

	void Move(float);
};

struct ViewerCamera : public Camera
{
	vec3 center;
	float radius, zoom;

	ViewerCamera();

	virtual void Mouse(float, float);
	virtual void Update(float);
};

#endif