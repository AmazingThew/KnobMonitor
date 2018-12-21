#include "KnobMonitor.h"
#include <cstdio>
#include <fstream>
#include <GLFW/glfw3.h>

#include "GLError.h"
#include "Knobs.h"


KnobMonitor::KnobMonitor()
{
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
	// glDrawArrays(GL_TRIANGLES, 0, 3);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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
		fprintf(stderr, "Failed to load shader file at %s", filePath.c_str());
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
	float angleA = Knobs::get(3, -PI2);
	float angleB = angleA + PI / 2;
	float angleC = angleB + PI / 2;
	float angleD = angleC + PI / 2;

	quadVerts.clear();
	quadVerts.push_back(glm::vec3(glm::cos(angleA), glm::sin(angleA), 0.0f));
	quadVerts.push_back(glm::vec3(glm::cos(angleB), glm::sin(angleB), 0.0f));
	quadVerts.push_back(glm::vec3(glm::cos(angleC), glm::sin(angleC), 0.0f));
	quadVerts.push_back(glm::vec3(glm::cos(angleD), glm::sin(angleD), 0.0f));
	
	quadColors.clear();
	quadColors.push_back(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	quadColors.push_back(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	quadColors.push_back(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	quadColors.push_back(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	
	quadIndices.clear();
	quadIndices.push_back(0);
	quadIndices.push_back(1);
	quadIndices.push_back(2);
	quadIndices.push_back(0);
	quadIndices.push_back(2);
	quadIndices.push_back(3);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIndicesBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, quadIndices.size() * sizeof(GLuint), &quadIndices[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, quadVertsBuffer);
	glBufferData(GL_ARRAY_BUFFER, quadVerts.size() * sizeof(glm::vec3), &quadVerts[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, quadColorsBuffer);
	glBufferData(GL_ARRAY_BUFFER, quadColors.size() * sizeof(glm::vec4), &quadColors[0], GL_DYNAMIC_DRAW);
}
