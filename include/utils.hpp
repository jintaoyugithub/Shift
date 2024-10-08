#include <cstdint>

namespace ips {
/* bgfx utils */

class AppGLFW {
public:
  AppGLFW(const char *title = "bgfx app glfw", uint32_t width = 800,
          uint32_t height = 600);
};
} // namespace ips
