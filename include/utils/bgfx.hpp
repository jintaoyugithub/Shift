#pragma once

#include <bgfx/bgfx.h>
#include <bx/bx.h>
#include <bx/file.h>
#include <initializer_list>
#include <string_view>

namespace shift
{
/// unpack input parameters
struct Args
{
    // constro
    Args(int _argc, const char **_argv);

    bgfx::RendererType::Enum _type;
    uint16_t _pciId;
};

/* bgfx utils */
/// Load program from shaders
// bgfx::ProgramHandle loadProgram(std::initializer_list<std::string_view> _names);
bgfx::ProgramHandle loadProgram(std::initializer_list<bx::StringView> _names);

/// Load resources from memory
// bgfx::ShaderHandle loadShader(const std::string_view& _name);
bgfx::ShaderHandle loadShader(const bx::StringView &_name);

/// Get native handle type
bgfx::NativeWindowHandleType::Enum getNativeWindowHandleType();

/// Used in imgui.cpp, for more details check https://github.com/bkaradzic/bgfx/blob/master/examples/common/bgfx_utils.h
inline bool checkAvailTransientBuffers(uint32_t _numVertices, const bgfx::VertexLayout &_layout, uint32_t _numIndices)
{
    return _numVertices == bgfx::getAvailTransientVertexBuffer(_numVertices, _layout) &&
           (0 == _numIndices || _numIndices == bgfx::getAvailTransientIndexBuffer(_numIndices));
}
} // namespace shift
