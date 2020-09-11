#include "geometry.h"

#include "shader.h"
#include "camera.h"
#include "material.h"

int Geometry::num = 0;

const std::vector<vec3> VERTICES = {
	vec3(-1.0, -1.0, 1.0),
	vec3(1.0, -1.0, 1.0),
	vec3(1.0, 1.0, 1.0),
	vec3(-1.0, 1.0, 1.0),
	vec3(-1.0, -1.0, -1.0),
	vec3(1.0, -1.0, -1.0),
	vec3(1.0, 1.0, -1.0),
	vec3(-1.0, 1.0, -1.0)};

const std::vector<GLuint> ELEMENTS = {
	0, 1, 2,
	2, 3, 0,
	1, 5, 6,
	6, 2, 1,
	7, 6, 5,
	5, 4, 7,
	4, 0, 3,
	3, 7, 4,
	4, 5, 1,
	1, 0, 4,
	3, 2, 6,
	6, 7, 3};

Geometry::Geometry()
{
	num++;
	for (const vec3 &v : VERTICES)
	{
		vertices.push_back(Vertex{v});
	}
	elements.insert(elements.end(), ELEMENTS.begin(), ELEMENTS.end());
}

Geometry::~Geometry()
{
	num--;
	log_("delete geometry");
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}

void Geometry::Clear(int num_vertices, int num_elements)
{
	vertices.clear();
	elements.clear();
	if (num_vertices)
		vertices.resize(num_vertices);
	if (num_elements)
		elements.resize(num_elements);
}

void Geometry::Draw(const mat4 &model)
{
	if (!created)
		return;

	bool uses_elements = elements.size() > 0;

	glBindVertexArray(vao);

	if (material)
	{
		material->Use();
		material->shader->SetMat4("model", model);
		material->shader->SetMat3("normalMatrix", transpose(inverse(mat3(camera->view * model))));
	}

	if (uses_elements)
		if (lines)
			glDrawElements(
				GL_LINES, (GLsizei)elements.size(), GL_UNSIGNED_INT, 0);
		else
			glDrawElements(
				GL_TRIANGLES, (GLsizei)elements.size(), GL_UNSIGNED_INT, 0);
	else
		glDrawArrays(
			GL_TRIANGLES, 0, (GLsizei)vertices.size());

	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}

void Geometry::SetupMesh()
{
	bool uses_elements = elements.size() > 0;

	aabb = AABB(0);

	for (Vertex &vertex : vertices)
		aabb.extend(vertex.position);

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(
		GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	if (uses_elements)
	{
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(GLuint), &elements[0], GL_STATIC_DRAW);
	}

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, uv));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(
		3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, color));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(
		4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tangent));

	glEnableVertexAttribArray(5);
	glVertexAttribPointer(
		5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, bitangent));

	glBindVertexArray(0);

	created = true;
}
