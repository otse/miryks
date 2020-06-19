#include "geometry"

#include "shader"
#include "camera"
#include "material"


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

geometry_t::geometry_t()
{
	for (const vec3 &v : VERTICES) {
		vertex_t ver;
		ver.position = v;
		vertices.push_back(ver);
		//vertices.push_back(vertex_t{v});
	}
	elements.insert(
		elements.end(), ELEMENTS.begin(), ELEMENTS.end());
}

geometry_t::~geometry_t()
{
	log_("delete geometry");

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}

void geometry_t::Clear(int num_vertices, int num_elements)
{
	vertices.clear();
	elements.clear();
	vertices.resize(num_vertices);
	elements.resize(num_elements);
}

void geometry_t::Draw(const mat4 &model)
{
	if (!created)
		return;

	bool uses_elements = elements.size() > 0;

	glBindVertexArray(vao);

	if (material)
	{
		material->Bind();

		material->shader->setMat4("model", model);
		material->shader->setMat3("normalMatrix", transpose(inverse(mat3(camera->view * model))));
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

void geometry_t::SetupMesh()
{
	bool uses_elements = elements.size() > 0;

	bb = aabb(0);

	for (vertex_t &vertex : vertices)
		bb.extend(vertex.position);

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex_t), &vertices[0], GL_STATIC_DRAW);

	if (uses_elements)
	{
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			elements.size() * sizeof(GLuint),
			&elements[0],
			GL_STATIC_DRAW);
	}

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *)offsetof(vertex_t, uv));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *)offsetof(vertex_t, normal));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(
		3, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *)offsetof(vertex_t, color));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(
		4, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *)offsetof(vertex_t, tangent));

	glEnableVertexAttribArray(5);
	glVertexAttribPointer(
		5, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *)offsetof(vertex_t, bitangent));

	glBindVertexArray(0);

	log_("created geometry");

	created = true;
}
