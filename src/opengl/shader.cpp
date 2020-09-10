#include "shader"

Shader *Shader::active = nullptr;

//shader_t *shader_current;

#include "types"

#include "camera"
#include "scene"

#include "files.h"

#include <glad/glad.h>

Shader::Shader()
{
	id = 0;
}

void Shader::Use()
{
	if (this == active)
		return;

	glUseProgram(id);

	scene->BindLights(this);

	SetMat4("view", camera->view);
	SetMat4("projection", camera->projection);

	active = this;
}

void Shader::Load(const string &vert, const string &frag)
{
	this->vert_path = vert;
	this->frag_path = frag;

	Compile();
}

void Shader::Compile()
{
	if (id)
		glDeleteProgram(id);

	int *i = new int;
	string vert_code = fread("gl/" + vert_path); // = toffshore("gl", vert_path, i);
	string frag_code = fread("gl/" + frag_path); // = toffshore("gl", frag_path, i);

	GLuint vertex, fragment;

	const char *vert_lval = vert_code.c_str();
	const char *frag_lval = frag_code.c_str();

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vert_lval, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &frag_lval, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");

	id = glCreateProgram();
	glAttachShader(id, vertex);
	glAttachShader(id, fragment);

	glLinkProgram(id);
	checkCompileErrors(id, "PROGRAM");

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	//log_("made ur shader");
}

void Shader::SetInt(const char *name, int value) const { glUniform1i(glGetUniformLocation(id, name), value); }
void Shader::SetBool(const char *name, bool value) const { glUniform1i(glGetUniformLocation(id, name), (int)value); }
void Shader::SetFloat(const char *name, float value) const { glUniform1f(glGetUniformLocation(id, name), value); }
void Shader::SetVec2(const char *name, const vec2 &value) const { glUniform2fv(glGetUniformLocation(id, name), 1, &value[0]); }
void Shader::SetVec3(const char *name, const vec3 &value) const { glUniform3fv(glGetUniformLocation(id, name), 1, &value[0]); }
void Shader::SetVec4(const char *name, const vec4 &value) const { glUniform4fv(glGetUniformLocation(id, name), 1, &value[0]); }
void Shader::SetMat2(const char *name, const mat2 &mat) const { glUniformMatrix2fv(glGetUniformLocation(id, name), 1, GL_FALSE, &mat[0][0]); }
void Shader::SetMat3(const char *name, const mat3 &mat) const { glUniformMatrix3fv(glGetUniformLocation(id, name), 1, GL_FALSE, &mat[0][0]); }
void Shader::SetMat4(const char *name, const mat4 &mat) const { glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, &mat[0][0]); }

void Shader::checkCompileErrors(GLuint shader, std::string type)
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
