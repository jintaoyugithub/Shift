#pragma once

#include <utils/common.hpp>
#include <bx/platform.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace shift {
    void* glfwNativeWindowHandle(GLFWwindow* _window);

    void* glfwNativeDisplayHandle();
} // namespace shift
