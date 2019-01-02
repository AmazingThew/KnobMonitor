#include <glad/glad.h>
#include <GLFW/glfw3.h> // GLFW helper library
#include <stdio.h>
#include <vector>

#include "Config.h"
#include "KnobMonitor.h"
#include "Knobs.h"


KnobMonitor* monitor;

void error_callback(int error, const char* description)
{
	fprintf(stderr, "ERROR: %s\n", description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int main() {
	glfwSetErrorCallback(error_callback);

	KnobConfig::Config* config = KnobConfig::getConfig();
	glm::vec2 resolution = glm::vec2(1024 / config->aspectCorrection.x, 1024 / config->aspectCorrection.y);

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
	monitor = new KnobMonitor(config, window);
	Knobs::start();
	

	// MAIN LOOP
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.08f, 0.15f, 0.22f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		monitor->update();
		monitor->draw();

		glfwSwapBuffers(window);
		glfwWaitEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, 1);
		}
	}


	glfwTerminate();
	return 0;
}
