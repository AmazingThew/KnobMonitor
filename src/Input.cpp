#include "Input.h"

Input::Input(GLFWwindow* window)
{
	this->window = window;
}


Input::~Input()
{
}

void Input::update()
{
	mouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	isMouseDown = mouseState == GLFW_PRESS;
	wasMousePressed = isMouseDown && prevMouseState == GLFW_RELEASE;
	wasMouseReleased = !isMouseDown && prevMouseState == GLFW_PRESS;
	prevMouseState = mouseState;

	leftState = glfwGetKey(window, GLFW_KEY_LEFT);
	isLeftDown = leftState == GLFW_PRESS;
	wasLeftPressed = isLeftDown && prevLeftState == GLFW_RELEASE;
	wasLeftReleased = !isLeftDown && prevLeftState == GLFW_PRESS;
	prevLeftState = leftState;

	rightState = glfwGetKey(window, GLFW_KEY_RIGHT);
	isRightDown = rightState == GLFW_PRESS;
	wasRightPressed = isRightDown && prevRightState == GLFW_RELEASE;
	wasRightReleased = !isRightDown && prevRightState == GLFW_PRESS;
	prevRightState = rightState;
}
