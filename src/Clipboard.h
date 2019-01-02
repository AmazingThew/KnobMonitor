#pragma once
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <WinUser.h>
#include <string>
#include <iomanip>
#include <sstream>

inline void copyToClipboard(GLFWwindow* window, std::string text)
{
	const char* cStr = text.c_str();
	const size_t len = strlen(cStr) + 1;
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
	memcpy(GlobalLock(hMem), cStr, len);
	GlobalUnlock(hMem);

	OpenClipboard(glfwGetWin32Window(window));
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hMem);
	CloseClipboard();
}

inline void copyToClipboard(GLFWwindow* window, float number, int precision = 8)
{
	std::stringstream stream;
	stream << std::fixed << std::setprecision(precision) << number;
	copyToClipboard(window, stream.str());
}
