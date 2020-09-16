#ifndef OPENGL_TYPES_H
#define OPENGL_TYPES_H

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/string_cast.hpp>

using namespace glm;

const float pif = pi<float>();

typedef char *ShaderSource[3];

struct AABB;

struct Geometry;
struct Material;
struct Camera;
struct FirstPersonCamera;
struct ViewerCamera;
struct Scene;
struct Group;
struct Renderable;
struct RenderItem;
struct Texture;
struct RenderTarget;
struct Shader;
struct Program;
struct PointLight;

extern Camera *camera;
extern Scene *scene;

void detectGlfwError();
void detectOpenGLError(const std::string = "?");

void opengl_gui();
void OGLGo();
void OGLReloadShaderSource();

struct RenderSettings
{
	bool diffuseMaps = true;
	bool normalMaps = true;
	bool specularMaps = true;
	bool glowMaps = true;
	bool dust = true;
	bool axisAlignedBoundingBoxes = false;
	bool orientedBoundingBoxes = false;
};
extern RenderSettings renderSettings;

#endif