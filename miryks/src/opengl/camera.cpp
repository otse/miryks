#include <opengl/camera.h>
#include <opengl/group.h>
#include <opengl/drawgroup.h>
#include <opengl/scene.h>

#include <miryks/miryks.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

Camera::Camera()
{
	pitch = -pif / 2;
	fov = 55;
	view = mat4(1);
	pos = vec3(0);
	disabled = false;
	group = new group_type;
	drawGroup = new drawgroup(group, mat4(1.0));
}

void Camera::SetProjection() {
	float aspect = (float)width / (float)height;

	projection = glm::perspective(radians(fov), aspect, 5.0f, 10000.0f);
}

void Camera::Wasd(float delta)
{
	if (disabled)
		return;
		
	auto forward = [&](float n) {
		pos.x += n * sin(yaw);
		pos.y += n * cos(yaw);
	};
	auto strafe = [&](float n) {
		pos.x += n * cos(-yaw);
		pos.y += n * sin(-yaw);
	};
	float speed = 500.f * delta;

	using namespace miryks::input;

	if (!holding_key("lshift"))
		speed /= 10;

	if (holding_key("w") && !holding_key("s"))
		forward(speed);
	if (holding_key("s") && !holding_key("w"))
		forward(-speed);

	if (holding_key("a") && !holding_key("d"))
		strafe(-speed);
	if (holding_key("d") && !holding_key("a"))
		strafe(speed);

	if (holding_key("r"))
		pos.z += speed / 2;
	if (holding_key("f"))
		pos.z -= speed / 2;
}

FirstPersonCamera::FirstPersonCamera() : Camera()
{
	eye = vec3(0);
	hands = new GroupBounded;
	hands->matrix = glm::translate(mat4(1.0), vec3(0, 0, -100));
	group->Add(hands);
}

void FirstPersonCamera::Mouse(float x, float y)
{
	if (disabled)
		return;
	const float sensitivity = .001f;
	yaw += x * sensitivity;
	pitch += y * sensitivity;
}

void FirstPersonCamera::Update(float time)
{
	if (!disabled)
		Wasd(time);

	while (yaw > 2 * pif)
		yaw -= 2 * pif;
	while (yaw < 0)
		yaw += 2 * pif;

	pitch = fmaxf(-pif, fminf(0, pitch));

	view = mat4(1.0f);
	view = glm::rotate(view, pitch, vec3(1, 0, 0));
	view = glm::rotate(view, yaw, vec3(0, 0, 1));
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
	pitch += y * sensitivity;
}

void ViewerCamera::Update(float time)
{
	if (!disabled)
		Wasd(time);

	view = mat4(1.0f);
	view = glm::rotate(view, pitch, vec3(1, 0, 0));
	view = glm::rotate(view, yaw, vec3(0, 0, 1));

	vec3 pan = vec3(0, 0, radius) * mat3(view);

	view = glm::translate(view, -pan);
	view = glm::translate(view, -pos);

	group->matrix = glm::inverse(view);
	group->Update(); // Important

	Camera::SetProjection();
}