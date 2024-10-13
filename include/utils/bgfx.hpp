#pragma once

#include <bgfx/bgfx.h>
#include <string_view>

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
	bgfx::ProgramHandle loadProgram(const std::string_view& _vsName, const std::string_view& _fsName);

	/// Load resources from memory
	bgfx::ShaderHandle loadShader(const std::string_view& _name);

	/// Get native handle type
	bgfx::NativeWindowHandleType::Enum getNativeWindowHandleType();
} // namespace shift
