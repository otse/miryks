#include <gloom/dark2.h>

#include <opengl/camera.h>
#include <opengl/group.h>
#include <opengl/scene.h>
#include <opengl/renderable.h>

#include <glm/gtc/matrix_transform.hpp>

double Camera::prev[2] = { 0, 0 };

Camera::Camera()
{
	view = mat4(1);
	pos = vec3(0);
	fzoom = 45;
	disabled = false;
	group = new Group;
}

void Camera::SetProjection() {
	float aspect = (float)gloom::width / (float)gloom::height;

	projection = perspective(radians(fzoom), aspect, 5.0f, 10000.0f);
}

FirstPersonCamera::FirstPersonCamera() : Camera()
{
	eye = vec3(0);
	w = a = s = d = r = f = false;
	shift = false;

	hands = new Group();
	hands->matrix = translate(mat4(1.0), vec3(0, 0, 0));
	Group *a = new Group;
	a->matrix = translate(mat4(1.0), vec3(-100, 0, 0));
	Group *b = new Group;
	b->matrix = translate(mat4(1.0), vec3(100, 0, 0));
	Group *c = new Group;
	c->matrix = translate(mat4(1.0), vec3(0, -100, 0));
	Group *d = new Group;
	d->matrix = translate(mat4(1.0), vec3(0, 100, 0));
	Group *e = new Group;
	e->matrix = translate(mat4(1.0), vec3(0, 0, -100));
	Group *f = new Group;
	f->matrix = translate(mat4(1.0), vec3(0, 0, 100));
	group->Add(hands);
	group->Add(a);
	group->Add(b);
	group->Add(c);
	group->Add(d);
	group->Add(e);
	group->Add(f);
	group->Update();
	renderable = new Renderable(mat4(1.0), group);
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
	view = rotate(view, fpitch, vec3(1, 0, 0));
	view = rotate(view, fyaw, vec3(0, 0, 1));
	view = translate(view, -pos - eye);

	group->matrix = glm::translate(mat4(1.0), -pos);
	group->Update();

	Camera::SetProjection();
}

void FirstPersonCamera::Move(float time)
{
	auto forward = [&](float n) {
		pos.x += n * sin(fyaw);
		pos.y += n * cos(fyaw);
	};

	auto strafe = [&](float n) {
		pos.x += n * cos(-fyaw);
		pos.y += n * sin(-fyaw);
	};

	float speed = 500 * time;

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
	view = rotate(view, pitch, vec3(1, 0, 0));
	view = rotate(view, yaw, vec3(0, 0, 1));

	vec3 pan = vec3(0, 0, radius) * mat3(view);

	view = translate(view, -pan);
	view = translate(view, -pos);

	Camera::SetProjection();
}