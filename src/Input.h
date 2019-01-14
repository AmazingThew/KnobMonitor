#pragma once
#include <GLFW/glfw3.h>

class Input
{
	GLFWwindow* window;

	int mouseState, prevMouseState;
	int leftState, prevLeftState;
	int rightState, prevRightState;

public:
	Input(GLFWwindow* window);
	~Input();

	void update();

	bool isMouseDown, wasMousePressed, wasMouseReleased;
	bool isLeftDown, wasLeftPressed, wasLeftReleased;
	bool isRightDown, wasRightPressed, wasRightReleased;
};

