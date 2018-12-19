#include "KnobMonitor.h"
#include <cstdio>
#include <fstream>
#include <GLFW/glfw3.h>

#include "GLError.h"
#include "Knobs.h"


KnobMonitor::KnobMonitor()
{
	basicShader = compileShader("./shaders/basic.vert", "./shaders/basic.frag");
	createTriangle();
}


KnobMonitor::~KnobMonitor()
{
}

void KnobMonitor::update()
{
	updateTriangle();
}

void KnobMonitor::draw()
{
	glUseProgram(basicShader);
	glBindVertexArray(triangleVao);
	glDrawArrays(GL_TRIANGLES, 0, 3);

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

void KnobMonitor::createTriangle()
{
	// float positions[] = {
	// 	0.0f,  0.5f,  0.0f,
	// 	0.5f, -0.5f,  0.0f,
	// 	-0.5f, -0.5f,  0.0f
	// };

	triangleVerts[0] = glm::vec3(0.0f, 0.5f, 0.0f);
	triangleVerts[1] = glm::vec3(0.5f, -0.5f, 0.0f);
	triangleVerts[2] = glm::vec3(-0.5f, -0.5f, 0.0f);

	float colors[] = {
		1.0f, 0.0f,  0.0f,
		0.0f, 1.0f,  0.0f,
		0.0f, 0.0f,  1.0f
	};

	triangleVertsVbo = 0;
	glGenBuffers(1, &triangleVertsVbo);
	// glBindBuffer(GL_ARRAY_BUFFER, triangleVertsVbo);
	// glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), triangleVerts, GL_DYNAMIC_DRAW);

	GLuint colors_vbo = 0;
	glGenBuffers(1, &colors_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), colors, GL_STATIC_DRAW);

	triangleVao = 0;
	glGenVertexArrays(1, &triangleVao);
	glBindVertexArray(triangleVao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVertsVbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
}

void KnobMonitor::updateTriangle()
{
	// float angleA = Knobs::get(3, -PI2);
	// float angleB = angleA + PI / 2;
	// float angleC = angleB + PI / 2;
	//
	// triangleVerts[0] = glm::vec3(glm::cos(angleA), glm::sin(angleA), 0.0f);
	// triangleVerts[1] = glm::vec3(glm::cos(angleB), glm::sin(angleB), 0.0f);
	// triangleVerts[2] = glm::vec3(glm::cos(angleC), glm::sin(angleC), 0.0f);
	//
	// glBindBuffer(GL_ARRAY_BUFFER, triangleVertsVbo);
	// glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), triangleVerts, GL_DYNAMIC_DRAW);

	float angleA = Knobs::get(3, -PI2);
	float angleB = angleA + PI / 2;
	float angleC = angleB + PI / 2;
	float angleD = angleC + PI / 2;

	quadVerts[0] = glm::vec3(glm::cos(angleA), glm::sin(angleA), 0.0f);
	quadVerts[1] = glm::vec3(glm::cos(angleB), glm::sin(angleB), 0.0f);
	quadVerts[2] = glm::vec3(glm::cos(angleC), glm::sin(angleC), 0.0f);
	quadVerts[3] = glm::vec3(glm::cos(angleD), glm::sin(angleD), 0.0f);

	quadIndices[0] = 0;
	quadIndices[1] = 1;
	quadIndices[2] = 2;
	quadIndices[3] = 0;
	quadIndices[4] = 2;
	quadIndices[5] = 3;

	glBindBuffer(GL_ARRAY_BUFFER, triangleVertsVbo);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), quadVerts, GL_DYNAMIC_DRAW);
}
