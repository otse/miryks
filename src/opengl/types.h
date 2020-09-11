#ifndef OPENGL_TYPES
#define OPENGL_TYPES

#include "../idiom.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace glm;

const float pif = pi<float>();

typedef char *ssrc[3];

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
struct Shader;
struct ShaderSource;
struct Program;
struct PointLight;

extern Camera *camera;
extern Scene *scene;

void detectGlfwError();
void detectOpenGLError(const string = "?");

void opengl_gui();
void OGLGo();
void OGLReloadShaderSource();

#endif