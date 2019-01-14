#include "KnobMonitor.h"
#include <cstdio>
#include <fstream>
#include <GLFW/glfw3.h>

#include "GLError.h"
#include "Knobs.h"
#include "Clipboard.h"
#include "stb_image.h"


KnobMonitor::KnobMonitor(KnobConfig* config, GLFWwindow* window)
{
	this->config = config;
	this->window = window;

	currentPage = config->currentPage();

	// basicShader = compileShader("D:\\Development\\C++\\KnobMonitor\\KnobMonitor\\shaders\\basic.vert", "D:\\Development\\C++\\KnobMonitor\\KnobMonitor\\shaders\\basic.frag");
	basicShader = compileShader("./assets/basic.vert", "./assets/basic.frag");
	textureShader = compileShader("./assets/basic.vert", "./assets/textured.frag");
	
	loadNumberTexture();

	dialMesh   = new Mesh();
	gaugeMesh  = new Mesh();
	numberMesh = new Mesh();

	generateGauges(gaugeMesh);
}


KnobMonitor::~KnobMonitor()
{
	delete numberMesh;
	delete gaugeMesh;
	delete dialMesh;
}

void KnobMonitor::update()
{
	currentPage = config->currentPage();

	buttonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	isButtonPressed = buttonState == GLFW_PRESS;
	wasButtonPressed = isButtonPressed && prevButtonState == GLFW_RELEASE;
	wasButtonReleased = !isButtonPressed && prevButtonState == GLFW_PRESS;
	prevButtonState = buttonState;
	
	double mouseScreenX, mouseScreenY;
	int screenWidth, screenHeight;
	glfwGetCursorPos(window, &mouseScreenX, &mouseScreenY);
	glfwGetWindowSize(window, &screenWidth, &screenHeight);
	glm::vec3 mousePos = glm::vec3(glm::vec2(mouseScreenX / screenWidth, 1.0f - mouseScreenY / screenHeight) * 2.0f - 1.0f, 0);
	
	hoveredIndex = -1;
	if (glm::abs(mousePos.x) <= 1 && glm::abs(mousePos.y) <= 1)
	{
		for (auto knob : *currentPage->knobs)
		{
			float distance = glm::length(knob.center - mousePos / currentPage->aspectCorrection);
			if (distance < currentPage->gaugeScale)
			{
				hoveredIndex = knob.index;
				break;
			}
		}
	
		if (wasButtonPressed)
			clickedIndex = hoveredIndex;
	
		if (wasButtonReleased)
		{
			if (hoveredIndex == clickedIndex && hoveredIndex > -1)
			{
				copyToClipboard(window, Knobs::get(hoveredIndex));
			}
			clickedIndex = -1;
		}
	}
	
	generateDials(dialMesh);
	generateGauges(gaugeMesh);
	generateNumbers(numberMesh);
}

void KnobMonitor::draw()
{
	gaugeMesh->draw(basicShader);
	dialMesh->draw(basicShader);
	numberMesh->draw(textureShader, numberTexture);
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

void KnobMonitor::loadNumberTexture()
{
	glGenTextures(1, &numberTexture);
	glBindTexture(GL_TEXTURE_2D, numberTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, numChannels;
	unsigned char *data = stbi_load("./assets/Numbers.png", &width, &height, &numChannels, 4);
	
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		fprintf(stderr, "Failed to load numberTexture\n");
	}

	stbi_image_free(data);

	check_gl_error();
}

void KnobMonitor::generateNumbers(Mesh* mesh)
{
	mesh->clear();

	for (auto knob : *currentPage->knobs)
	{
		float centerOffset = (baseKnobRadius * ringPadding) * currentPage->gaugeScale;
		bool hovered = knob.index == hoveredIndex;
		bool clicked = knob.index == clickedIndex;
		glm::vec4 color = hovered && (clicked || clickedIndex == -1) ? red : white;
		float clickScaleFactor = clicked && hovered ? clickScale : 1;

		glm::vec3 center = knob.center - glm::vec3(0, centerOffset * clickScaleFactor, 0);
		center *= currentPage->aspectCorrection;
		appendNumberQuad(mesh, knob.index, color, center, numberSize * clickScaleFactor);
	}

	mesh->apply();
}

void KnobMonitor::generateGauges(Mesh* mesh)
{
	mesh->clear();
	for (auto knob : *currentPage->knobs)
	{
		appendGauge(gaugeMesh, knob.index, knob.center, currentPage->gaugeScale);
	}
	mesh->apply();
}

void KnobMonitor::generateDials(Mesh* mesh)
{
	dialMesh->clear();
	for (auto knob : *currentPage->knobs)
	{
		appendDial(dialMesh, knob.index, knob.center, currentPage->gaugeScale, Knobs::get(knob.index));
	}
	dialMesh->apply();
}

void KnobMonitor::appendGauge(Mesh* mesh, int index, glm::vec3 center, float scale)
{
	bool hovered = index == hoveredIndex;
	bool clicked = index == clickedIndex;
	glm::vec4 color = hovered && (clicked || clickedIndex == -1) ? red : white;
	scale *= clicked && hovered ? clickScale : 1;


	float radius = baseKnobRadius * scale;
	float width = baseDialWidth * scale;

	float tickRadius = radius + width * ringPadding;
	float tickLength = width * ringPadding;
	float tickWidth = arcWidth;
	float thinTickWidth = tickWidth * 0.7f;

	appendArc(mesh, color, color, center, tickRadius, arcWidth, resolution, -PI / 2.0f - deadSplit / 2.0f, -(PI2 - deadSplit));

	float tickSplit = -(PI2 - deadSplit) / (numTicks - 1);
	for (int i = 0; i < numTicks; i++)
	{
		float angle = -PI / 2 - deadSplit / 2 + tickSplit * i;
		float length = i % 2 == 0 ? tickLength : tickLength * 0.5f;
		float tickLineWidth = i % 4 == 0 ? tickWidth : thinTickWidth;
		glm::vec3 start = center + glm::vec3(glm::cos(angle), glm::sin(angle), 0) * tickRadius;
		glm::vec3 end = center + glm::vec3(glm::cos(angle), glm::sin(angle), 0) * (tickRadius + length);

		appendLine(mesh, color, color, start, end, tickLineWidth);
	}

	// appendArc(mesh, red, red, center, scale, thinTickWidth, 64, 0, PI2);
}

void KnobMonitor::appendDial(Mesh* mesh, int index, glm::vec3 center, float scale, float progress)
{
	bool hovered = index == hoveredIndex;
	bool clicked = index == clickedIndex;
	scale *= clicked && hovered ? clickScale : 1;

	glm::vec4 dialColor = red;
	glm::vec4 hubColor = hovered && (clicked || clickedIndex == -1) ? red : white;;

	float radius = baseKnobRadius * scale;
	float width = baseDialWidth * scale;
	float circleWidth = 0.005f;

	appendArc(mesh, dialColor, dialColor, center, radius, width, resolution, -PI / 2 - deadSplit / 2, -(PI2 - deadSplit) * progress);
	appendArc(mesh, hubColor, hubColor, center, glm::mix(width / 2, radius - width*ringPadding, progress), circleWidth, resolution, 0, PI2);
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
		) * currentPage->aspectCorrection);
		mesh->vertices.push_back(glm::vec3(
			glm::cos(angle) * (radius + width / 2) + center.x,
			glm::sin(angle) * (radius + width / 2) + center.y,
			center.z
		) * currentPage->aspectCorrection);

		float colorInterpolant = (float)i / (resolution - 1);
		mesh->colors.push_back(glm::mix(startColor, endColor, colorInterpolant));
		mesh->colors.push_back(glm::mix(startColor, endColor, colorInterpolant));

		mesh->uvs.push_back(glm::vec2(0, colorInterpolant));
		mesh->uvs.push_back(glm::vec2(1, colorInterpolant));
	}
}

void KnobMonitor::appendLine(Mesh* mesh, glm::vec4 startColor, glm::vec4 endColor, glm::vec3 start, glm::vec3 end, float width)
{
	glm::vec3 lineWidth = glm::vec3(width / 2, width / 2, 0) * currentPage->aspectCorrection;


	glm::vec2 segment = end - start;
	glm::vec2 direction = glm::normalize(segment);
	glm::vec3 ortho = glm::vec3(-direction.y, direction.x, 0) * lineWidth;

	start *= currentPage->aspectCorrection;
	end   *= currentPage->aspectCorrection;

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

	mesh->uvs.push_back(glm::vec2(0, 0));
	mesh->uvs.push_back(glm::vec2(1, 0));
	mesh->uvs.push_back(glm::vec2(1, 1));
	mesh->uvs.push_back(glm::vec2(0, 1));
}

void KnobMonitor::appendBox(Mesh* mesh, glm::vec4 color, glm::vec3 center, glm::vec2 dimensions, float width)
{
	glm::vec2 halfDimensions = dimensions / 2.0f;
	glm::vec3 dl = center + glm::vec3(-halfDimensions.x, -halfDimensions.y, 0);
	glm::vec3 dr = center + glm::vec3(halfDimensions.x, -halfDimensions.y, 0);
	glm::vec3 ul = center + glm::vec3(-halfDimensions.x, halfDimensions.y, 0);
	glm::vec3 ur = center + glm::vec3(halfDimensions.x, halfDimensions.y, 0);
	appendLine(mesh, color, color, dl, dr, width);
	appendLine(mesh, color, color, dr, ur, width);
	appendLine(mesh, color, color, ur, ul, width);
	appendLine(mesh, color, color, ul, dl, width);
}

void KnobMonitor::appendNumberQuad(Mesh* mesh, int index, glm::vec4 color, glm::vec3 center, float scale)
{
	float gridSize = 1.0f / 8.0f;
	float left = gridSize * (index % 8);
	float right = left + gridSize;
	float up = gridSize * (index / 8);
	float down = up + gridSize;

	int startIndex = static_cast<int>(mesh->vertices.size());
	mesh->indices.push_back(startIndex + 0);
	mesh->indices.push_back(startIndex + 2);
	mesh->indices.push_back(startIndex + 3);
	mesh->indices.push_back(startIndex + 0);
	mesh->indices.push_back(startIndex + 1);
	mesh->indices.push_back(startIndex + 2);

	mesh->vertices.push_back(center + glm::vec3(-scale, -scale, 0) * currentPage->aspectCorrection);
	mesh->vertices.push_back(center + glm::vec3(-scale,  scale, 0) * currentPage->aspectCorrection);
	mesh->vertices.push_back(center + glm::vec3( scale,  scale, 0) * currentPage->aspectCorrection);
	mesh->vertices.push_back(center + glm::vec3( scale, -scale, 0) * currentPage->aspectCorrection);

	mesh->colors.push_back(color);
	mesh->colors.push_back(color);
	mesh->colors.push_back(color);
	mesh->colors.push_back(color);

	mesh->uvs.push_back(glm::vec2(left, down));
	mesh->uvs.push_back(glm::vec2(left, up));
	mesh->uvs.push_back(glm::vec2(right, up));
	mesh->uvs.push_back(glm::vec2(right, down));
}
