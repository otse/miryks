#ifndef OPENGL_SHADER_H
#define OPENGL_SHADER_H

#include <OpenGL/Types.h>

#include <OpenGL/Camera.h>
#include <OpenGL/Scene.h>

#include <Gloom/Files.h>

#include <glad/glad.h>

void SetShaderSources();

extern ShaderSource simple, basic, fxs, postquad;

struct Shader
{
	std::string CommonHeader();
	
	static std::map<std::string, Shader *> shaders;

	static Shader *active;

	bool dust = true;

	ShaderSource *const src;
	GLuint id;

	std::string header;

	Shader(ShaderSource *);

	void Compile();

	void Use();

	void SetInt(const char *, int) const;
	void SetBool(const char *, bool) const;
	void SetFloat(const char *, float) const;
	void SetVec2(const char *, const vec2 &) const;
	void SetVec3(const char *, const vec3 &) const;
	void SetVec4(const char *, const vec4 &) const;
	void SetMat2(const char *, const mat2 &) const;
	void SetMat3(const char *, const mat3 &) const;
	void SetMat4(const char *, const mat4 &) const;

private:
	void checkCompileErrors(GLuint, std::string);
};
#endif