#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class Mesh
{
public:
	Mesh();
	~Mesh();

	void clear();
	void apply();
	void draw(GLuint shader);
	void draw(GLuint shader, GLuint texture);

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec4> colors;
	std::vector<glm::vec2> uvs;
	std::vector<GLuint> indices;
	GLuint vertexBuffer, uvBuffer, colorBuffer, indexBuffer, vao;
};

