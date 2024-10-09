#include <bgfx/bgfx.h>
#include <iostream>
#include <utilsBgfx.hpp>
#include <utilsGLFW.hpp>

int main(int _agrc, const char **_argv) {
  std::cout << "Hello tri" << std::endl;

  if (!glfwInit()) {
    return -1;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  GLFWwindow *window = glfwCreateWindow(800, 600, "test", nullptr, nullptr);

  bgfx::Init init;
  init.type = bgfx::RendererType::Count;
  init.vendorId = BGFX_PCI_ID_NONE;
  init.platformData.nwh = ips::glfwNativeWindowHandle(window);
  init.platformData.ndt = ips::glfwNativeDisplayHandle();
  init.platformData.type = ips::getNativeWindowHandleType();
  init.resolution.width = 800;
  init.resolution.height = 600;
  bgfx::init(init);

  // Set view 0 clear state.
  bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f,
                     0);

  std::cout << bgfx::getRendererType() << std::endl;

  bgfx::setViewRect(0, 0, 0, 800, 600);
  bgfx::touch(0);

  while (!glfwWindowShouldClose(window)) {
    glfwSwapBuffers(window);
    glfwPollEvents();

    bgfx::frame();
  }

  return 0;
}
