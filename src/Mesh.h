#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

class Mesh
{
public:
	Mesh();
	~Mesh();

	void clear();
	void apply();
	void draw(GLuint shader);

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec4> colors;
	std::vector<GLuint> indices;
	GLuint vertexBuffer, colorBuffer, indexBuffer, vao;
};

