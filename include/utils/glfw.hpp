#pragma once

#include <utils/common.hpp>
#include <bx/platform.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#if !(GLFW_VERSION_MAJOR > 3 || (GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 4) )
#	error "GLFW 3.4 or later is required"
#endif // GLFW_VERSION_*

#if BX_PLATFORM_LINUX
#	define GLFW_EXPOSE_NATIVE_WAYLAND
#	define GLFW_EXPOSE_NATIVE_X11
#	define GLFW_EXPOSE_NATIVE_GLX
#elif BX_PLATFORM_OSX
#	define GLFW_EXPOSE_NATIVE_COCOA
#	define GLFW_EXPOSE_NATIVE_NSGL
#elif BX_PLATFORM_WINDOWS
#	define GLFW_EXPOSE_NATIVE_WIN32
#	define GLFW_EXPOSE_NATIVE_WGL
#endif //
#include <GLFW/glfw3native.h>


namespace shift {
    /// --- Callback functions
	void keyCb(GLFWwindow* window, int key, int scancode, int action, int modes);
	void charCb(GLFWwindow* window, unsigned int codepoint);
	void charModsCb(GLFWwindow* window, unsigned int codepoint, int mods);
	void mouseButtonCb(GLFWwindow* window, int button, int action, int modes);
	void cursorPosCb(GLFWwindow* window, double xpos, double ypos);
	void curosrEnterCb(GLFWwindow* window, int entered);
	void scroolCb(GLFWwindow* window, double xoffset, double yoffset);
	void dropCb(GLFWwindow* window, int count, const char ** paths);
	void windowSizeCb(GLFWwindow* window, unsigned int width, unsigned int height);

    // Get native window handle
    inline void* glfwNativeWindowHandle(GLFWwindow* _window) {
#if BX_PLATFORM_LINUX
        if (GLFW_PLATFORM_WAYLAND == glfwGetPlatform()) {
            return glfwGetWaylandWindow(_window);
        }

        return (void*)uintptr_t(glfwGetX11Window(_window));
#elif BX_PLATFORM_OSX
        return glfwGetCocoaWindow(_window);
#elif BX_PLATFORM_WINDOWS
        return glfwGetWin32Window(_window);
#endif // BX_PLATFORM_
    }

    inline void* glfwNativeDisplayHandle() {
#if BX_PLATFORM_LINUX
        if (GLFW_PLATFORM_WAYLAND == glfwGetPlatform()) {
            return glfwGetWaylandDisplay();
        }

        return glfwGetX11Display();
#elif BX_PLATFORM_OSX || BX_PLATFORM_WINDOWS
        return nullptr;
#endif // BX_PLATFORM_
    }
} // namespace shift
