#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#define PI 3.14159265359f
#define PI2 6.28318530718f

class KnobMonitor
{
public:
	KnobMonitor();
	~KnobMonitor();

	void update();
	void draw();

private:
	float dt;
	glm::vec3* quadVerts = new glm::vec3[4];
	GLuint* quadIndices = new GLuint[4];
	GLuint quadVertsBuffer, quadIndicesBuffer, quadVao;

	glm::vec3* triangleVerts = new glm::vec3[3];
	GLuint triangleVertsVbo, triangleVao;
	GLuint basicShader;

	GLuint compileShader(std::string filePath, GLenum shaderType);
	GLuint compileShader(std::string vertexFilePath, std::string fragmentFilePath);
	void createTriangle();
	void updateTriangle();
};

