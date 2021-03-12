#ifndef OPENGL_CAMERA_H
#define OPENGL_CAMERA_H

// part of gloom

#include <OpenGL/Types.h>

#include <glm/gtc/matrix_transform.hpp>

struct Camera
{
public:
	float yaw, pitch;
	Camera();

	Group *group;
	DrawGroup *drawGroup;

	vec3 pos;
	mat4 view, /*matrix,*/ projection;

	float fzoom;
	bool disabled;

	static double prev[2];

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