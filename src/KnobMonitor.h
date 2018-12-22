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
	glm::vec4 red   = glm::vec4(0.9, 0.1, 0.15, 1);
	glm::vec4 white = glm::vec4(1, 0.96, 0.91, 1);
	glm::vec4 navy  = glm::vec4(0.08, 0.15, 0.22, 1);

	float baseKnobRadius = 0.5f;
	float baseDialWidth = 0.15f;
	float ringPadding = 1.3;
	float deadSplit = 1;
	float arcWidth = 0.005f;
	int numTicks = 17;
	int resolution = 64;

	std::vector<glm::vec3> quadVerts;
	std::vector<glm::vec4> quadColors;
	std::vector<GLuint> quadIndices;
	GLuint quadVertsBuffer, quadColorsBuffer, quadIndicesBuffer, quadVao;

	GLuint basicShader;

	GLuint compileShader(std::string filePath, GLenum shaderType);
	GLuint compileShader(std::string vertexFilePath, std::string fragmentFilePath);
	void createGeometry();
	void updateGeometry();

	void appendGauge(std::vector<glm::vec3>* vertices, std::vector<GLuint>* indices, std::vector<glm::vec4>* colors, glm::vec3 center, float scale);
	void appendDial(std::vector<glm::vec3>* vertices, std::vector<GLuint>* indices, std::vector<glm::vec4>* colors, glm::vec3 center, float scale, float progress);

	void appendArc(std::vector<glm::vec3>* vertices, std::vector<GLuint>* indices, std::vector<glm::vec4>* colors, glm::vec4 startColor, glm::vec4 endColor, glm::vec3 center, float radius, float width, int resolution, float startAngle, float subtense);
	void appendLine(std::vector<glm::vec3>* vertices, std::vector<GLuint>* indices, std::vector<glm::vec4>* colors, glm::vec4 startColor, glm::vec4 endColor, glm::vec3 start, glm::vec3 end, float width);
};

