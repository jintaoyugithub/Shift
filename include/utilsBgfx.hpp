#pragma once

#include <bgfx/bgfx.h>

namespace ips {
/// Get native handle type
static bgfx::NativeWindowHandleType::Enum getNativeWindowHandleType() {
  return bgfx::NativeWindowHandleType::Default;
}

} // namespace ips
