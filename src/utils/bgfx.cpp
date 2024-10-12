#include <utils/bgfx.hpp>

namespace shift {
static const bgfx::Memory* loadMem(const std::string_view& _filePath) {
	return nullptr;
}

static bgfx::ShaderHandle loadShader(bx::FileReaderI* _reader, const std::string_view& _name) {
	std::string_view shaderPath;

	switch (bgfx::getRendererType()) {
	case bgfx::RendererType::Noop:
	case bgfx::RendererType::Direct3D11:
	case bgfx::RendererType::Direct3D12: shaderPath = "";  break;
	case bgfx::RendererType::Agc:
	case bgfx::RendererType::Gnm:        shaderPath;  break;
	case bgfx::RendererType::Metal:      shaderPath; break;
	case bgfx::RendererType::Nvn:        shaderPath;   break;
	case bgfx::RendererType::OpenGL:     shaderPath;  break;
	case bgfx::RendererType::OpenGLES:   shaderPath;  break;
	case bgfx::RendererType::Vulkan:     shaderPath; break;

	case bgfx::RendererType::Count:
		BX_ASSERT(false, "You should not be here!");
		break;
	};

	bgfx::ShaderHandle handle = bgfx::createShader(loadMem(_name));
	return handle;
}

bgfx::NativeWindowHandleType::Enum getNativeWindowHandleType() {
	return bgfx::NativeWindowHandleType::Default;
}

bgfx::ShaderHandle loadShader(const std::string_view& _name) {
	return loadShader(_name);
}
}