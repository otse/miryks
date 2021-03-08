#include <Gloom/Gloom.h>

#include <OpenGL/Camera.h>
#include <OpenGL/Group.h>
#include <OpenGL/DrawGroup.h>
#include <OpenGL/Scene.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

double Camera::prev[2] = { 0, 0 };

Camera::Camera()
{
	view = mat4(1);
	pos = vec3(0);
	fzoom = 50;
	disabled = false;
	group = new Group;
	drawGroup = new DrawGroup(group);
}

void Camera::SetProjection() {
	float aspect = (float)gloom::width / (float)gloom::height;

	projection = glm::perspective(radians(fzoom), aspect, 5.0f, 10000.0f);
}

FirstPersonCamera::FirstPersonCamera() : Camera()
{
	eye = vec3(0);
	w = a = s = d = r = f = false;
	shift = false;

	hands = new Group;
	hands->matrix = glm::translate(mat4(1.0), vec3(0, 0, -100));
	group->Add(hands);
}

void FirstPersonCamera::Mouse(float x, float y)
{
	if (disabled)
		return;
	const float sensitivity = .001f;
	fyaw += x * sensitivity;
	fpitch += y * sensitivity;
	Camera::prev[0] = x;
	Camera::prev[1] = y;
}

void FirstPersonCamera::Update(float time)
{
	if (disabled) {
		Camera::SetProjection();
		return;
	}

	Move(time);

	while (fyaw > 2 * pif)
		fyaw -= 2 * pif;
	while (fyaw < 0)
		fyaw += 2 * pif;

	fpitch = fmaxf(-pif, fminf(0, fpitch));

	view = mat4(1.0f);
	view = glm::rotate(view, fpitch, vec3(1, 0, 0));
	view = glm::rotate(view, fyaw, vec3(0, 0, 1));
	view = glm::translate(view, -pos);
	view = glm::translate(view, -eye);

	//matrix = view; // Why here
	//matrix = glm::inverse(matrix);

	group->matrix = glm::inverse(view);
	
	group->Update(); // Important
	
	drawGroup->Reset();

	// printf("hands matrix world %s\n", glm::to_string(vec3(hands->matrixWorld[3])));

	Camera::SetProjection();
}

void FirstPersonCamera::Move(float delta)
{
	auto forward = [&](float n) {
		pos.x += n * sin(fyaw);
		pos.y += n * cos(fyaw);
	};

	auto strafe = [&](float n) {
		pos.x += n * cos(-fyaw);
		pos.y += n * sin(-fyaw);
	};

	float speed = 500.f * delta;

	if (shift)
		speed /= 10;

	if (w && !s)
		forward(speed);
	if (s && !w)
		forward(-speed / 2);

	if (a && !d)
		strafe(-speed);
	if (d && !a)
		strafe(speed);

	if (r)
		pos.z += speed / 2;
	if (f)
		pos.z -= speed / 2;
}

ViewerCamera::ViewerCamera() : Camera()
{
	center = vec3(0);
	radius = 100;
	yaw = 0;
	pitch = 0;
}

void ViewerCamera::Mouse(float x, float y)
{
	if (disabled)
		return;
	const float sensitivity = .001f;
	yaw += x * sensitivity;
	pitch -= y * sensitivity;
}

void ViewerCamera::Update(float time)
{
	view = mat4(1.0f);
	view = glm::rotate(view, pitch, vec3(1, 0, 0));
	view = glm::rotate(view, yaw, vec3(0, 0, 1));

	vec3 pan = vec3(0, 0, radius) * mat3(view);

	view = glm::translate(view, -pan);
	view = glm::translate(view, -pos);

	Camera::SetProjection();
}