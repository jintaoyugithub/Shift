#include <utils/bgfx.hpp>

#include <bx/commandline.h>
#include <spdlog/spdlog.h>

#include <iostream>
#include <filesystem>
#include <fstream>

namespace shift {
	static bgfx::ProgramHandle loadProgram_t(std::initializer_list<std::string_view> _shaderNames) {
		if (_shaderNames.size() > 0) {
			auto itr = _shaderNames.begin();
			switch (_shaderNames.size()) {
			case 1:
				spdlog::info("Creating compute shader");
				bgfx::ShaderHandle csShader = loadShader(*itr);
				return bgfx::createProgram(csShader);
			case 2:
				spdlog::info("Creating vertex and fragment shader");
				bgfx::ShaderHandle vsShader = loadShader(*itr);
				bgfx::ShaderHandle fsShader = loadShader(*(++itr));
				return bgfx::createProgram(vsShader, fsShader);
			default:
				spdlog::error("incorrect shader name number");
				break;
			}
		} else {
			spdlog::error("require at least one shader name");
			return BGFX_INVALID_HANDLE;
		}
	}

	bgfx::ProgramHandle loadProgram(std::initializer_list<std::string_view> _shaderNames) {
		return loadProgram_t(_shaderNames);
	}

	static const bgfx::Memory* loadMem(const std::string_view& _fileName) {
		std::ifstream file(static_cast<std::string>(_fileName), std::ios::binary | std::ios::ate);
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);
		const bgfx::Memory* mem = bgfx::alloc(uint32_t(size + 1));
		if (file.read((char*)mem->data, size))
		{
			mem->data[mem->size - 1] = '\0';
			return mem;
		}
		//spdlog::error("fail to open the file: { }", static_cast<std::string>(_fileName));
		return nullptr;
	}

	/// Load shader
	static bgfx::ShaderHandle loadShader_t(const std::string_view& _name) {
		std::filesystem::path shaderPath = "shaders";

		switch (bgfx::getRendererType())
		{
		case bgfx::RendererType::Noop:
		case bgfx::RendererType::Direct3D11:
		case bgfx::RendererType::Direct3D12: shaderPath /= "dx11";  break;
		case bgfx::RendererType::Agc:
		case bgfx::RendererType::Gnm:        shaderPath /= "pssl";  break;
		case bgfx::RendererType::Metal:      shaderPath /= "metal"; break;
		case bgfx::RendererType::Nvn:        shaderPath /= "nvn";   break;
		case bgfx::RendererType::OpenGL:     shaderPath /= "glsl";  break;
		case bgfx::RendererType::OpenGLES:   shaderPath /= "essl";  break;
		case bgfx::RendererType::Vulkan:     shaderPath /= "spirv"; break;

		case bgfx::RendererType::Count:
			BX_ASSERT(false, "You should not be here!");
			break;
		}

		char fileName[512];
		bx::strCopy(fileName, BX_COUNTOF(fileName), static_cast<std::string>(_name).c_str());
		bx::strCat(fileName, BX_COUNTOF(fileName), ".bin");

		shaderPath /= fileName;

		//spdlog::info("Loading shader from: { }", shaderPath.string());

		bgfx::ShaderHandle handle = bgfx::createShader(loadMem(shaderPath.string()));

		return handle;
	}

	bgfx::ShaderHandle loadShader(const std::string_view& _name) {
		return loadShader_t(_name);
	}

	/// Load native window hanlde type
	static bgfx::NativeWindowHandleType::Enum getNativeWindowHandleType_t() {
		return bgfx::NativeWindowHandleType::Default;
	}

	bgfx::NativeWindowHandleType::Enum getNativeWindowHandleType() {
		return getNativeWindowHandleType_t();
	}

	/// Unpack args
	Args::Args(int _argc, const char** _argv) : 
		_type(bgfx::RendererType::Enum::Count),
		_pciId(BGFX_PCI_ID_NONE) {
		
		bx::CommandLine cmdLine(_argc, _argv);

		if (cmdLine.hasArg("gl"))
		{
			_type = bgfx::RendererType::OpenGL;
		}
		else if (cmdLine.hasArg("vk"))
		{
			_type = bgfx::RendererType::Vulkan;
		}
		else if (cmdLine.hasArg("noop"))
		{
			_type = bgfx::RendererType::Noop;
		}
		else if (cmdLine.hasArg("d3d11"))
		{
			_type = bgfx::RendererType::Direct3D11;
		}
		else if (cmdLine.hasArg("d3d12"))
		{
			_type = bgfx::RendererType::Direct3D12;
		}
		else if (BX_ENABLED(BX_PLATFORM_OSX))
		{
			if (cmdLine.hasArg("mtl"))
			{
				_type = bgfx::RendererType::Metal;
			}
		}

		if (cmdLine.hasArg("amd"))
		{
			_pciId = BGFX_PCI_ID_AMD;
		}
		else if (cmdLine.hasArg("nvidia"))
		{
			_pciId = BGFX_PCI_ID_NVIDIA;
		}
		else if (cmdLine.hasArg("intel"))
		{
			_pciId = BGFX_PCI_ID_INTEL;
		}
		else if (cmdLine.hasArg("sw"))
		{
			_pciId = BGFX_PCI_ID_SOFTWARE_RASTERIZER;
		}
	}

}

