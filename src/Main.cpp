#include <glad/glad.h>
#include <GLFW/glfw3.h> // GLFW helper library
#include <stdio.h>
#include <vector>

#include "KnobMonitor.h"
#include "Knobs.h"


KnobMonitor* monitor;

void error_callback(int error, const char* description)
{
	fprintf(stderr, "ERROR: %s\n", description);
}

int main() {
	glfwSetErrorCallback(error_callback);

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
	GLFWwindow* window = glfwCreateWindow(1024, 1024, "Knob Monitor", NULL, NULL);
	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}
	glfwSetWindowAspectRatio(window, 1, 1);
	glfwMakeContextCurrent(window);

	// Start GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		fprintf(stderr, "ERROR: Failed to initialize GLAD\n");
		return -1;
	}


	// INITIALIZE
	monitor = new KnobMonitor();
	Knobs::start();
	

	// MAIN LOOP
	while (!glfwWindowShouldClose(window))
	{
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
