#include <glad/glad.h>
#include <GLFW/glfw3.h> // GLFW helper library
#include <stdio.h>
#include <vector>

#include "Config.h"
#include "KnobMonitor.h"
#include "Knobs.h"

#define BASE_RESOLUTION 700


KnobMonitor* monitor;
Input* input;

void error_callback(int error, const char* description)
{
	fprintf(stderr, "ERROR: %s\n", description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void change_aspect(KnobConfig* config, GLFWwindow* window)
{
	int prevWidth, prevHeight, baseResolution;
	glfwGetWindowSize(window, &prevWidth, &prevHeight);
	baseResolution = glm::max(prevWidth, prevHeight);

	glm::vec2 resolution = glm::vec2(baseResolution / config->currentPage()->aspectCorrection.x, baseResolution / config->currentPage()->aspectCorrection.y);
	glfwSetWindowSize(window, resolution.x, resolution.y);
	glfwSetWindowAspectRatio(window, resolution.x, resolution.y);

}

int main() {
	glfwSetErrorCallback(error_callback);

	KnobConfig* config = new KnobConfig();
	glm::vec2 resolution = glm::vec2(BASE_RESOLUTION / config->currentPage()->aspectCorrection.x, BASE_RESOLUTION / config->currentPage()->aspectCorrection.y);

	// start GL context and O/S window using the GLFW helper library
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	// uncomment these lines if on Apple OS X
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_SAMPLES, 8);
	GLFWwindow* window = glfwCreateWindow(resolution.x, resolution.y, "Knob Monitor", NULL, NULL);
	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetWindowAspectRatio(window, resolution.x, resolution.y);
	glfwMakeContextCurrent(window);

	// Start GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		fprintf(stderr, "ERROR: Failed to initialize GLAD\n");
		return -1;
	}


	// INITIALIZE
	input = new Input(window);
	monitor = new KnobMonitor(config, window, input);
	Knobs::start();
	

	// MAIN LOOP
	while (!glfwWindowShouldClose(window))
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glClearColor(0.08f, 0.15f, 0.22f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		input->update();
		monitor->update();
		monitor->draw();

		glfwSwapBuffers(window);

		if (input->wasRightPressed) {
			config->incrementPage();
			change_aspect(config, window);
		}

		if (input->wasLeftPressed) {
			config->decrementPage();
			change_aspect(config, window);
		}

		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, 1);
		}

		glfwWaitEvents();
	}


	glfwTerminate();
	return 0;
}
