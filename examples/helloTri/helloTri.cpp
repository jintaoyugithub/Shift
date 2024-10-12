#include <bgfx/bgfx.h>
#include <iostream>
#include <utils/bgfx.hpp>
#include <utils/glfw.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct PosColorVertex {
    float _x;
    float _y;
    float _z;
    uint32_t _rgba;

    static void init() {
        _layout
            .begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();
    };

    static bgfx::VertexLayout _layout;
};

bgfx::VertexLayout PosColorVertex::_layout;

PosColorVertex cubeVertices[] = {
    {-1.0f,  1.0f,  1.0f, 0xff000000 },
    { 1.0f,  1.0f,  1.0f, 0xff0000ff },
    {-1.0f, -1.0f,  1.0f, 0xff00ff00 },
    { 1.0f, -1.0f,  1.0f, 0xff00ffff },
    {-1.0f,  1.0f, -1.0f, 0xffff0000 },
    { 1.0f,  1.0f, -1.0f, 0xffff00ff },
    {-1.0f, -1.0f, -1.0f, 0xffffff00 },
    { 1.0f, -1.0f, -1.0f, 0xffffffff },
};

static const uint16_t s_cubeTriList[] = { 
    2, 1, 0, 
    2, 3, 1,
    5, 6, 4,
    7, 6, 5,
    4, 2, 0,
    6, 2, 4,
    3, 5, 1,
    3, 7, 5,
    1, 4, 0,
    1, 5, 4,
    6, 3, 2,
    7, 3, 6
};


int main(int _agrc, const char **_argv) {
  std::cout << "Hello tri" << std::endl;

  if (!glfwInit()) {
    return -1;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  GLFWwindow *window = glfwCreateWindow(800, 600, "test", nullptr, nullptr);

  bgfx::Init init;
  init.type = bgfx::RendererType::OpenGL;
  init.vendorId = BGFX_PCI_ID_NONE;
  init.platformData.nwh = shift::glfwNativeWindowHandle(window);
  init.platformData.ndt = shift::glfwNativeDisplayHandle();
  init.platformData.type = shift::getNativeWindowHandleType();
  init.resolution.width = 800;
  init.resolution.height = 600;
  init.resolution.reset = BGFX_RESET_VSYNC;
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


  // shutdown
  bgfx::shutdown();
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
