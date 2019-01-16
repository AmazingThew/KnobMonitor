#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <glad/glad.h>
#include <GLFW/glfw3.h> // GLFW helper library
#include <stdio.h>

#include "Config.h"
#include "KnobMonitor.h"
#include "Knobs.h"
#include "Utils/stb_image.h"

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

	int width  = static_cast<int>(baseResolution / config->currentPage()->aspectCorrection.x);
	int height = static_cast<int>(baseResolution / config->currentPage()->aspectCorrection.y);
	glfwSetWindowSize(window, width, height);
	glfwSetWindowAspectRatio(window, width, height);

}

void load_icons(GLFWwindow* window)
{
	GLFWimage icons[5];

	int iconWidth, iconHeight, iconChannels;
		
	unsigned char *iconData256 = stbi_load("./assets/icon/Icon256.png", &iconWidth, &iconHeight, &iconChannels, 4);
	icons[0] = GLFWimage {
		iconWidth,
		iconHeight,
		iconData256,
	};

	unsigned char *iconData128 = stbi_load("./assets/icon/Icon128.png", &iconWidth, &iconHeight, &iconChannels, 4);
	icons[1] = GLFWimage{
		iconWidth,
		iconHeight,
		iconData128,
	};

	unsigned char *iconData64  = stbi_load("./assets/icon/Icon64.png", &iconWidth, &iconHeight, &iconChannels, 4);
	icons[2] = GLFWimage{
		iconWidth,
		iconHeight,
		iconData64,
	};

	unsigned char *iconData32  = stbi_load("./assets/icon/Icon32.png", &iconWidth, &iconHeight, &iconChannels, 4);
	icons[3] = GLFWimage{
		iconWidth,
		iconHeight,
		iconData32,
	};

	unsigned char *iconData16  = stbi_load("./assets/icon/Icon16.png", &iconWidth, &iconHeight, &iconChannels, 4);
	icons[4] = GLFWimage{
		iconWidth,
		iconHeight,
		iconData16,
	};

	glfwSetWindowIcon(window, 5, icons);

	stbi_image_free(iconData256);
	stbi_image_free(iconData128);
	stbi_image_free(iconData64);
	stbi_image_free(iconData32);
	stbi_image_free(iconData16);
}

void load_icon(std::string path)
{
	int iconWidth, iconHeight, iconChannels;
	unsigned char *iconData = stbi_load("./assets/Icon.png", &iconWidth, &iconHeight, &iconChannels, 4);
	GLFWimage icon = {
		iconWidth,
		iconHeight,
		iconData
	};
}

int main() {
	glfwSetErrorCallback(error_callback);

	KnobConfig* config = new KnobConfig();

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
	int width  = static_cast<int>(BASE_RESOLUTION / config->currentPage()->aspectCorrection.x);
	int height = static_cast<int>(BASE_RESOLUTION / config->currentPage()->aspectCorrection.y);
	GLFWwindow* window = glfwCreateWindow(width, height, "Knob Monitor", NULL, NULL);
	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetWindowAspectRatio(window, width, height);
	glfwMakeContextCurrent(window);

	// Start GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		fprintf(stderr, "ERROR: Failed to initialize GLAD\n");
		return -1;
	}

	load_icons(window);

	// INITIALIZE
	input = new Input(window);
	monitor = new KnobMonitor(config, window, input);
	Knobs::start();
	

	// MAIN LOOP
	while (!glfwWindowShouldClose(window))
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glClearColor(0.0471200980f, 0.118134134f, 0.149606302f, 1);
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
