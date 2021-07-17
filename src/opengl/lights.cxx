#include <OpenGL/Lights.h>

// https://community.khronos.org/t/uniform-buffer-objects-dynamic-sized-arrays-and-lights/70415

const float PI = glm::pi<float>();
const float PI2 = glm::pi<float>() * 2.f;

Light::Light()
{
	matrix = mat4(1.0f);
	color = vec3(1);
	intensity = 1.f;
}

PointLight::PointLight()
{
	distance = 0.f;
	decay = 1.f;
}

SpotLight::SpotLight()
{
	distance = 0.f;
	penumbra = 0.f;
	angle = pif / 3.f;
	decay = 1.f;
	target = vec3(0);
}

DirectionalLight::DirectionalLight()
{
	target = vec3(0);
}

float Light::CalcDist() const
{
	return glm::distance(vec3(matrix[3]), camera_current->pos);
}

void Light::Calc() // Unused
{
	vec3 position, color;

	position = vec3(matrix[3]) * mat3(inverse(camera_current->view));
	position += vec3(camera_current->view[3]);
	color = color * intensity;
}