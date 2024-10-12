#pragma once

#include <bgfx/bgfx.h>
#include <bx/allocator.h>
#include <bx/readerwriter.h>
#include <string_view>
#include <bx/file.h>
#include <bx/readerwriter.h>
#include <bx/string.h>
#include <iostream>

namespace shift {
/* bgfx utils */ 
/// Get native handle type
bgfx::NativeWindowHandleType::Enum getNativeWindowHandleType();

/// Load resources from memory
bgfx::ShaderHandle loadShader(const std::string_view& _name);

bgfx::ProgramHandle loadProgram(const std::string_view& _vsName, const std::string_view& _fsName);

} // namespace shift
