#include "KnobMonitor.h"
#include <cstdio>
#include <fstream>
#include <GLFW/glfw3.h>

#include "GLError.h"
#include "Knobs.h"


KnobMonitor::KnobMonitor()
{
	// basicShader = compileShader("D:\\Development\\C++\\KnobMonitor\\KnobMonitor\\shaders\\basic.vert", "D:\\Development\\C++\\KnobMonitor\\KnobMonitor\\shaders\\basic.frag");
	basicShader = compileShader("./shaders/basic.vert", "./shaders/basic.frag");

	dialMesh = new Mesh();
	gaugeMesh = new Mesh();
	appendGauge(gaugeMesh, glm::vec3(0, 0, 0), 1);
	gaugeMesh->apply();
}


KnobMonitor::~KnobMonitor()
{
	delete gaugeMesh;
	delete dialMesh;
}

void KnobMonitor::update()
{
	dialMesh->clear();
	appendDial(dialMesh, glm::vec3(0, 0, 0), Knobs::get(7), Knobs::get(3));
	dialMesh->apply();
}

void KnobMonitor::draw()
{
	gaugeMesh->draw(basicShader);
	dialMesh->draw(basicShader);
}

GLuint KnobMonitor::compileShader(std::string filePath, GLenum shaderType)
{
	std::string shaderString;
	std::ifstream sourceFile(filePath.c_str());
	if (sourceFile) {
		// Read file
		shaderString.assign((std::istreambuf_iterator< char >(sourceFile)), std::istreambuf_iterator< char >());

		// Compile shader
		const GLchar* shaderSource = shaderString.c_str();
		GLuint index = glCreateShader(shaderType);
		glShaderSource(index, 1, &shaderSource, NULL);
		glCompileShader(index);

		// Check for errors
		GLint isCompiled = 0;
		glGetShaderiv(index, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(index, GL_INFO_LOG_LENGTH, &maxLength);

			GLchar* errorLog = new char[maxLength];
			glGetShaderInfoLog(index, maxLength, &maxLength, errorLog);

			fprintf(stderr, "Shader error: %s\n%s\n", filePath.c_str(), errorLog);

			glDeleteShader(index);
			return -1;
		}

		return index;
	}
	else
	{
		fprintf(stderr, "Failed to load shader file at %s\n", filePath.c_str());
		return -1;
	}
}

GLuint KnobMonitor::compileShader(std::string vertexFilePath, std::string fragmentFilePath)
{
	GLuint vert = compileShader(vertexFilePath, GL_VERTEX_SHADER);
	GLuint frag = compileShader(fragmentFilePath, GL_FRAGMENT_SHADER);

	if (vert < 0 || frag < 0)
		return -1;

	GLuint program = glCreateProgram();
	glAttachShader(program, frag);
	glAttachShader(program, vert);
	glLinkProgram(program);

	return program;
}

void KnobMonitor::appendGauge(Mesh* mesh, glm::vec3 center, float scale)
{
	glm::vec4 color = white;


	float radius = baseKnobRadius * scale;
	float width = baseDialWidth * scale;

	float tickRadius = radius + width * ringPadding;
	float tickLength = width * ringPadding;
	float tickWidth = 0.03f;
	float thinTickWidth = 0.005f;

	appendArc(mesh, color, color, center, tickRadius, arcWidth, resolution, -PI / 2.0f - deadSplit / 2.0f, -(PI2 - deadSplit));

	float tickSplit = -(PI2 - deadSplit) / (numTicks - 1);
	for (int i = 0; i < numTicks; i++)
	{
		float angle = -PI / 2 - deadSplit / 2 + tickSplit * i;
		float length = i % 2 == 0 ? tickLength : tickLength * 0.5f;
		float tickLineWidth = i % 4 == 0 ? tickWidth : thinTickWidth;
		glm::vec3 start = center + glm::vec3(glm::cos(angle), glm::sin(angle), 0) * tickRadius;
		glm::vec3 end = center + glm::vec3(glm::cos(angle), glm::sin(angle), 0) * (tickRadius+length);

		appendLine(mesh, color, color, start, end, tickLineWidth);
	}
}

void KnobMonitor::appendDial(Mesh* mesh, glm::vec3 center, float scale, float progress)
{
	glm::vec4 color = red;

	float radius = baseKnobRadius * scale;
	float width = baseDialWidth * scale;
	float circleWidth = 0.005f;

	appendArc(mesh, color, color, center, radius, width, resolution, -PI / 2 - deadSplit / 2, -(PI2 - deadSplit) * progress);
	appendArc(mesh, white, white, center, glm::mix(width / 2, radius - width*ringPadding, progress), circleWidth, resolution, 0, PI2);
}

void KnobMonitor::appendArc(Mesh* mesh, glm::vec4 startColor, glm::vec4 endColor, glm::vec3 center, float radius, float width, int resolution, float startAngle, float subtense)
{
	int totalVerts = static_cast<int>(mesh->vertices.size());
	for (int i = 0; i<resolution - 1; i++)
	{
		int startIndex = totalVerts + i * 2;
		mesh->indices.push_back(startIndex + 0);
		mesh->indices.push_back(startIndex + 3);
		mesh->indices.push_back(startIndex + 2);
		mesh->indices.push_back(startIndex + 3);
		mesh->indices.push_back(startIndex + 0);
		mesh->indices.push_back(startIndex + 1);
	}

	float angleStep = subtense / (resolution - 1);
	for (int i=0; i<resolution; i++)
	{
		float angle = angleStep * i + startAngle;
		mesh->vertices.push_back(glm::vec3(
			glm::cos(angle) * (radius - width / 2) + center.x,
			glm::sin(angle) * (radius - width / 2) + center.y,
			center.z
		));
		mesh->vertices.push_back(glm::vec3(
			glm::cos(angle) * (radius + width / 2) + center.x,
			glm::sin(angle) * (radius + width / 2) + center.y,
			center.z
		));

		float colorInterpolant = (float)i / (resolution - 1);
		mesh->colors.push_back(glm::mix(startColor, endColor, colorInterpolant));
		mesh->colors.push_back(glm::mix(startColor, endColor, colorInterpolant));
	}
}

void KnobMonitor::appendLine(Mesh* mesh, glm::vec4 startColor, glm::vec4 endColor, glm::vec3 start, glm::vec3 end, float width)
{
	glm::vec2 segment = end - start;
	glm::vec2 direction = glm::normalize(segment);
	glm::vec3 ortho = glm::vec3(-direction.y, direction.x, 0) * width / 2.0f;

	int startIndex = static_cast<int>(mesh->vertices.size());
	mesh->indices.push_back(startIndex + 0);
	mesh->indices.push_back(startIndex + 2);
	mesh->indices.push_back(startIndex + 3);
	mesh->indices.push_back(startIndex + 0);
	mesh->indices.push_back(startIndex + 1);
	mesh->indices.push_back(startIndex + 2);

	mesh->vertices.push_back(glm::vec3(start+ortho));
	mesh->vertices.push_back(glm::vec3(start-ortho));
	mesh->vertices.push_back(glm::vec3(end-ortho));
	mesh->vertices.push_back(glm::vec3(end+ortho));

	mesh->colors.push_back(startColor);
	mesh->colors.push_back(startColor);
	mesh->colors.push_back(endColor);
	mesh->colors.push_back(endColor);
}
