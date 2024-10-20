#pragma once

#include <bgfx/bgfx.h>
#include <string_view>
#include <initializer_list>

namespace shift {
	/// unpack input parameters
	struct Args {
		// constro
		Args(int _argc, const char** _argv);

		bgfx::RendererType::Enum _type;
		uint16_t _pciId;
	};

	/* bgfx utils */ 
	/// Load program from shaders
	bgfx::ProgramHandle loadProgram(std::initializer_list<std::string_view> _names);

	/// Load resources from memory
	bgfx::ShaderHandle loadShader(const std::string_view& _name);

	/// Get native handle type
	bgfx::NativeWindowHandleType::Enum getNativeWindowHandleType();
} // namespace shift
