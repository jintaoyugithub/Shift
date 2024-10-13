#include <utils/glfw.hpp>

#if !(GLFW_VERSION_MAJOR > 3 ||                                                \
      (GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 4))
#error "GLFW 3.4 or later is required"
#endif // GLFW_VERSION_*

#if BX_PLATFORM_LINUX
#define GLFW_EXPOSE_NATIVE_WAYLAND
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#elif BX_PLATFORM_OSX
#define GLFW_EXPOSE_NATIVE_COCOA
#define GLFW_EXPOSE_NATIVE_NSGL
#elif BX_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#endif //
#include <GLFW/glfw3native.h>

namespace shift {
    /// Get native window handle
    static void* glfwNativeWindowHandle_t(GLFWwindow* _window) {
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

    void* glfwNativeWindowHandle(GLFWwindow* _window) {
        return glfwNativeWindowHandle_t(_window);
    }

    static void* glfwNativeDisplayHandle_t() {
#if BX_PLATFORM_LINUX
        if (GLFW_PLATFORM_WAYLAND == glfwGetPlatform()) {
            return glfwGetWaylandDisplay();
        }

        return glfwGetX11Display();
#elif BX_PLATFORM_OSX || BX_PLATFORM_WINDOWS
        return nullptr;
#endif // BX_PLATFORM_
    }

    void* glfwNativeDisplayHandle() {  
        return glfwNativeDisplayHandle_t();
    }
} // namespace shift
