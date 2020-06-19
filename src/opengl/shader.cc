#include "shader"

shader_t *shader_t::BOUND = nullptr;

//shader_t *shader_current;

#include "types"

#include "camera"
#include "scene"

#include "files.hpp"

#include <glad/glad.h>

shader_t::shader_t()
{
	id = 0;
}

void shader_t::Reload()
{
	Load2();
}

void shader_t::Use()
{
	if (this == BOUND)
		return;

	glUseProgram(id);

	scene->BindLights(this);

	setMat4("view", camera->view);
	setMat4("projection", camera->projection);

	BOUND = this;
}

void shader_t::Load(const string &vert, const string &frag)
{
	this->vert_path = vert;
	this->frag_path = frag;

	Load2();
}

void shader_t::Load2()
{
	if (id)
	{
		glDeleteProgram(id);
	}

	int *i = new int;
	string vert_code = fread("gl/" + vert_path); // = toffshore("gl", vert_path, i);
	string frag_code = fread("gl/" + frag_path); // = toffshore("gl", frag_path, i);

	GLuint vertex, fragment;

	const char *vert_code_char = vert_code.c_str();
	const char *frag_code_char = frag_code.c_str();

	//log_(vert_code_char);

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vert_code_char, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &frag_code_char, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");

	id = glCreateProgram();
	glAttachShader(id, vertex);
	glAttachShader(id, fragment);

	glLinkProgram(id);
	checkCompileErrors(id, "PROGRAM");

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	log_("made ur shader");
}

void shader_t::setBool(const char *name, bool value) const
{
	glUniform1i(glGetUniformLocation(id, name), (int)value);
}

void shader_t::setInt(const char *name, int value) const
{
	glUniform1i(glGetUniformLocation(id, name), value);
}

void shader_t::setFloat(const char *name, float value) const
{
	glUniform1f(glGetUniformLocation(id, name), value);
}

void shader_t::setVec2(const char *name, const vec2 &value) const
{
	glUniform2fv(glGetUniformLocation(id, name), 1, &value[0]);
}

void shader_t::setVec2(const char *name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(id, name), x, y);
}

void shader_t::setVec3(const char *name, const vec3 &value) const
{
	glUniform3fv(glGetUniformLocation(id, name), 1, &value[0]);
}

void shader_t::setVec3(const char *name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(id, name), x, y, z);
}

void shader_t::setVec4(const char *name, const vec4 &value) const
{
	glUniform4fv(glGetUniformLocation(id, name), 1, &value[0]);
}

void shader_t::setVec4(const char *name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(id, name), x, y, z, w);
}

void shader_t::setMat2(const char *name, const mat2 &mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(id, name), 1, GL_FALSE, &mat[0][0]);
}

void shader_t::setMat3(const char *name, const mat3 &mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(id, name), 1, GL_FALSE, &mat[0][0]);
}

void shader_t::setMat4(const char *name, const mat4 &mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, &mat[0][0]);
}

void shader_t::checkCompileErrors(GLuint shader, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			log_("glGetShaderInfoLog for ", type, "\n", infoLog, "\n");
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			log_("glGetProgramInfoLog for ", type, "\n", infoLog, "\n");
		}
	}
}
