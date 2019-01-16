#pragma once
#include "Mesh.h"
#include "Config.h"
#include "Input.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <GLFW/glfw3.h>

#define PI 3.14159265359f
#define PI2 6.28318530718f

class KnobMonitor
{
public:
	KnobMonitor(KnobConfig* config, GLFWwindow* window, Input* input);
	~KnobMonitor();

	void update();
	void draw();

	glm::vec3 rgb2hsv(glm::vec3 c)
	{
		glm::vec4 K = glm::vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
		glm::vec4 p = glm::mix(glm::vec4(glm::vec2(c.b, c.g), glm::vec2(K.w, K.z)), glm::vec4(glm::vec2(c.g, c.b), glm::vec2(K.x, K.y)), glm::step(c.b, c.g));
		glm::vec4 q = glm::mix(glm::vec4(glm::vec3(p.x, p.y, p.w), c.r), glm::vec4(c.r, glm::vec3(p.y, p.z, p.x)), glm::step(p.x, c.r));

		float d = q.x - glm::min(q.w, q.y);
		float e = 1.0e-10f;
		return glm::vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
	}

	glm::vec3 hsv2rgb(glm::vec3 c)
	{
		glm::vec4 K = glm::vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
		glm::vec3 p = glm::abs(glm::fract(glm::vec3(c.x) + glm::vec3(K.x, K.y, K.z)) * 6.0f - glm::vec3(K.w));
		return c.z * glm::mix(glm::vec3(K.x), glm::clamp(p - glm::vec3(K.x), 0.0f, 1.0f), c.y);
	}

private:
	KnobConfig* config;
	KnobConfig::Page* currentPage;
	GLFWwindow* window;
	Input* input;

	// glm::vec4 red   = glm::vec4(0.9, 0.1, 0.15, 1);
	glm::vec4 white = glm::vec4(1, 0.96, 0.91, 1);
	glm::vec4 navy  = glm::vec4(0.08, 0.15, 0.22, 1);

	float hueShiftPerTick = 0.004f;
	float dialGradientShift = 0.05511811f;
	float horizontalHueFactor = 0.03937008f;
	float hueShift = 0.22834645f;

	float accentHueBase = 0.98f;
	float accentSat = 0.89f;
	float accentVal = 0.9f;

	float baseKnobRadius = 0.5f;
	float baseDialWidth = 0.15f;
	float ringPadding = 1.3f;
	float deadSplit = 1;
	float arcWidth = 0.005f;
	float clickScale = 0.91f;
	float numberSize = 0.04724409f;
	int numTicks = 17;
	int resolution = 64;

	int hoveredIndex = -1;
	int clickedIndex = -1;

	Mesh* numberMesh;
	Mesh* gaugeMesh;
	Mesh* dialMesh;

	GLuint basicShader;
	GLuint textureShader;

	GLuint numberTexture;

	GLuint compileShader(std::string filePath, GLenum shaderType);
	GLuint compileShader(std::string vertexFilePath, std::string fragmentFilePath);

	void loadNumberTexture();

	void generateNumbers(Mesh* mesh);
	void generateGauges(Mesh* mesh);
	void generateDials(Mesh* mesh);

	void appendGauge(Mesh* mesh, KnobConfig::Knob knob, float scale);
	void appendDial(Mesh* mesh, KnobConfig::Knob knob, float scale);

	void appendArc(Mesh* mesh, glm::vec4 startColor, glm::vec4 endColor, glm::vec3 center, float radius, float width, int resolution, float startAngle, float subtense);
	void appendLine(Mesh* mesh, glm::vec4 startColor, glm::vec4 endColor, glm::vec3 start, glm::vec3 end, float width);
	void appendBox(Mesh* mesh, glm::vec4 color, glm::vec3 center, glm::vec2 dimensions, float width);
	void appendNumberQuad(Mesh* mesh, int index, glm::vec4 color, glm::vec3 center, float scale);

	glm::vec4 KnobMonitor::getAccentColor(KnobConfig::Knob knob, float additionalShift = 0);

	float remap(float inMin, float inMax, float outMin, float outMax, float value)
	{
		return outMin + (value - inMin) * (outMax - outMin) / (inMax - inMin);
	}
};

