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
	createGeometry();
}


KnobMonitor::~KnobMonitor()
{
}

void KnobMonitor::update()
{
	updateGeometry();
}

void KnobMonitor::draw()
{
	glUseProgram(basicShader);
	 glBindVertexArray(quadVao);
	glDrawElements(GL_TRIANGLES, quadIndices.size(), GL_UNSIGNED_INT, 0);

	check_gl_error();
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

void KnobMonitor::createGeometry()
{
	glGenVertexArrays(1, &quadVao);
	glBindVertexArray(quadVao);
	glGenBuffers(1, &quadVertsBuffer);
	glGenBuffers(1, &quadColorsBuffer);
	glGenBuffers(1, &quadIndicesBuffer);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIndicesBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, quadVertsBuffer);
	GLint posAttrib = glGetAttribLocation(basicShader, "PositionAttribute");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, quadColorsBuffer);
	GLint colAttrib = glGetAttribLocation(basicShader, "ColorAttribute");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE, 0, 0);
}

void KnobMonitor::updateGeometry()
{
	quadVerts.clear();
	quadIndices.clear();
	quadColors.clear();

	glm::vec4 startColor = glm::vec4(0, 0, 1, 1);
	glm::vec4 endColor = glm::vec4(1, 0, 0, 1);
	glm::vec3 center = glm::vec3(0, 0, 0);
	float radius = 0.25f;
	int resolution = 128;
	float width = Knobs::get(2);
	float startAngle = 0;
	float subtense = Knobs::get(3, PI2);

	appendArc(&quadVerts, &quadIndices, &quadColors, startColor, endColor, center, radius, width, resolution, startAngle, subtense);

	float lineAngle = Knobs::get(7, PI2);
	float lineLength = Knobs::get(6);
	float lineWidth = Knobs::get(5);
	appendLine(&quadVerts, &quadIndices, &quadColors, startColor, endColor, glm::vec3(0, 0, 0), glm::vec3(glm::cos(lineAngle), glm::sin(lineAngle), 0)*lineLength, lineWidth);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIndicesBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, quadIndices.size() * sizeof(GLuint), &quadIndices[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, quadVertsBuffer);
	glBufferData(GL_ARRAY_BUFFER, quadVerts.size() * sizeof(glm::vec3), &quadVerts[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, quadColorsBuffer);
	glBufferData(GL_ARRAY_BUFFER, quadColors.size() * sizeof(glm::vec4), &quadColors[0], GL_DYNAMIC_DRAW);
}

void KnobMonitor::appendArc(std::vector<glm::vec3>* vertices, std::vector<GLuint>* indices, std::vector<glm::vec4>* colors,
	glm::vec4 startColor, glm::vec4 endColor, glm::vec3 center, float radius, float width, int resolution, float startAngle, float subtense)
{
	int totalVerts = vertices->size();
	for (int i = 0; i<resolution - 1; i++)
	{
		int startIndex = totalVerts + i * 2;
		indices->push_back(startIndex + 0);
		indices->push_back(startIndex + 3);
		indices->push_back(startIndex + 2);
		indices->push_back(startIndex + 3);
		indices->push_back(startIndex + 0);
		indices->push_back(startIndex + 1);
	}

	float angleStep = subtense / (resolution - 1);
	for (int i=0; i<resolution; i++)
	{
		float angle = angleStep * i + startAngle;
		vertices->push_back(glm::vec3(
			glm::cos(angle) * (radius - width / 2) + center.x,
			glm::sin(angle) * (radius - width / 2) + center.y,
			center.z
		));
		vertices->push_back(glm::vec3(
			glm::cos(angle) * (radius + width / 2) + center.x,
			glm::sin(angle) * (radius + width / 2) + center.y,
			center.z
		));

		float colorInterpolant = (float)i / (resolution - 1);
		colors->push_back(glm::mix(startColor, endColor, colorInterpolant));
		colors->push_back(glm::mix(startColor, endColor, colorInterpolant));
	}
}

void KnobMonitor::appendLine(std::vector<glm::vec3>* vertices, std::vector<GLuint>* indices,
	std::vector<glm::vec4>* colors, glm::vec4 startColor, glm::vec4 endColor, glm::vec3 start, glm::vec3 end, float width)
{
	glm::vec2 segment = end - start;
	glm::vec2 direction = glm::normalize(segment);
	glm::vec3 ortho = glm::vec3(-direction.y, direction.x, 0) * width / 2.0f;

	int startIndex = vertices->size();
	indices->push_back(startIndex + 0);
	indices->push_back(startIndex + 2);
	indices->push_back(startIndex + 3);
	indices->push_back(startIndex + 0);
	indices->push_back(startIndex + 1);
	indices->push_back(startIndex + 2);

	vertices->push_back(glm::vec3(start+ortho));
	vertices->push_back(glm::vec3(start-ortho));
	vertices->push_back(glm::vec3(end-ortho));
	vertices->push_back(glm::vec3(end+ortho));

	colors->push_back(startColor);
	colors->push_back(startColor);
	colors->push_back(endColor);
	colors->push_back(endColor);
}
