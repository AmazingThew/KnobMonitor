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
	std::vector<glm::vec3> quadVerts;
	std::vector<glm::vec4> quadColors;
	std::vector<GLuint> quadIndices;
	GLuint quadVertsBuffer, quadColorsBuffer, quadIndicesBuffer, quadVao;

	GLuint basicShader;

	GLuint compileShader(std::string filePath, GLenum shaderType);
	GLuint compileShader(std::string vertexFilePath, std::string fragmentFilePath);
	void createGeometry();
	void updateGeometry();
};

