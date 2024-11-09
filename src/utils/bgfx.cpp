#include <utils/bgfx.hpp>

#include <bx/commandline.h>
#include <bx/sort.h>
#include <bx/allocator.h>

#include <spdlog/spdlog.h>

#include <iostream>
#include <filesystem>
#include <fstream>

namespace shift {
	/// <summary>
	/// Set up for using the functionalities provided by bx
	/// Check https://github.com/bkaradzic/bgfx/blob/master/examples/common/entry/entry.cpp for details
	/// </summary>
	extern bx::AllocatorI* getDefaultAllocator();
	bx::AllocatorI* _allocator = getDefaultAllocator();

	typedef bx::StringT<&_allocator> String;
	static String _currentDir;

	class FileReader : public bx::FileReader
	{
		typedef bx::FileReader super;

	public:
		virtual bool open(const bx::FilePath& _filePath, bx::Error* _err) override
		{
			String filePath(_currentDir);
			filePath.append(_filePath);
			return super::open(filePath.getPtr(), _err);
		}
	};

	class FileWriter : public bx::FileWriter
	{
		typedef bx::FileWriter super;

	public:
		virtual bool open(const bx::FilePath& _filePath, bool _append, bx::Error* _err) override
		{
			String filePath(_currentDir);
			filePath.append(_filePath);
			return super::open(filePath.getPtr(), _append, _err);
		}
	};

	static bx::FileReaderI* _fileReader = BX_NEW(_allocator, FileReader);
	static bx::FileWriterI* _fileWriter = BX_NEW(_allocator, FileWriter);
	/// end

	/// <summary>
	/// New way to load mem
	/// </summary>
	static const bgfx::Memory* loadMem(bx::FileReaderI* _reader, const bx::FilePath& _filePath) {
		if (bx::open(_reader, _filePath))
		{
			uint32_t size = (uint32_t)bx::getSize(_reader);
			const bgfx::Memory* mem = bgfx::alloc(size + 1);
			bx::read(_reader, mem->data, size, bx::ErrorAssert{});
			bx::close(_reader);
			mem->data[mem->size - 1] = '\0';
			return mem;
		}

		spdlog::error("Fail to load {}", _filePath.getCPtr());
		return nullptr;
	}
	

	/// <summary>
	/// Old way to load mem
	/// </summary>
	//static const bgfx::Memory* loadMem(const std::string_view& _fileName) {
	//	std::ifstream file(static_cast<std::string>(_fileName), std::ios::binary | std::ios::ate);
	//	std::streamsize size = file.tellg();
	//	file.seekg(0, std::ios::beg);
	//	const bgfx::Memory* mem = bgfx::alloc(uint32_t(size + 1));
	//	if (file.read((char*)mem->data, size))
	//	{
	//		mem->data[mem->size - 1] = '\0';
	//		return mem;
	//	}
	//	//spdlog::error("fail to open the file: {}", static_cast<std::string>(_fileName));
	//	return nullptr;
	//}



	/// <summary>
	/// New way of loading shader
	/// </summary>
	static bgfx::ShaderHandle loadShader(bx::FileReaderI* _reader, const bx::StringView& _name) {
		// Specify the runtime shader folders
		bx::FilePath filePath("shaders/");

		switch (bgfx::getRendererType())
		{
		case bgfx::RendererType::Noop:
		case bgfx::RendererType::Direct3D11:
		case bgfx::RendererType::Direct3D12: filePath.join("dx11");  break;
		case bgfx::RendererType::Agc:
		case bgfx::RendererType::Gnm:        filePath.join("pssl");  break;
		case bgfx::RendererType::Metal:      filePath.join("metal"); break;
		case bgfx::RendererType::Nvn:        filePath.join("nvn");   break;
		case bgfx::RendererType::OpenGL:     filePath.join("glsl");  break;
		case bgfx::RendererType::OpenGLES:   filePath.join("essl");  break;
		case bgfx::RendererType::Vulkan:     filePath.join("spirv"); break;

		case bgfx::RendererType::Count:
			BX_ASSERT(false, "You should not be here!");
			break;
		}

		char fileName[512];
		bx::strCopy(fileName, BX_COUNTOF(fileName), _name);
		bx::strCat(fileName, BX_COUNTOF(fileName), ".bin");

		filePath.join(fileName);

		bgfx::ShaderHandle handle = bgfx::createShader(loadMem(_reader, filePath.getCPtr()));
		bgfx::setName(handle, _name.getPtr(), _name.getLength());

		return handle;
	}

	bgfx::ShaderHandle loadShader(const bx::StringView& _shaderNames) {
		return loadShader(_fileReader, _shaderNames);
	}


	/// <summary>
	/// Old way to load shader
	/// </summary>
	//static bgfx::ShaderHandle loadShader_t(const std::string_view& _name) {
	//	std::filesystem::path shaderPath = "shaders";

	//	switch (bgfx::getRendererType())
	//	{
	//	case bgfx::RendererType::Noop:
	//	case bgfx::RendererType::Direct3D11:
	//	case bgfx::RendererType::Direct3D12: shaderPath /= "dx11";  break;
	//	case bgfx::RendererType::Agc:
	//	case bgfx::RendererType::Gnm:        shaderPath /= "pssl";  break;
	//	case bgfx::RendererType::Metal:      shaderPath /= "metal"; break;
	//	case bgfx::RendererType::Nvn:        shaderPath /= "nvn";   break;
	//	case bgfx::RendererType::OpenGL:     shaderPath /= "glsl";  break;
	//	case bgfx::RendererType::OpenGLES:   shaderPath /= "essl";  break;
	//	case bgfx::RendererType::Vulkan:     shaderPath /= "spirv"; break;

	//	case bgfx::RendererType::Count:
	//		BX_ASSERT(false, "You should not be here!");
	//		break;
	//	}

	//	char fileName[512];
	//	bx::strCopy(fileName, BX_COUNTOF(fileName), static_cast<std::string>(_name).c_str());
	//	bx::strCat(fileName, BX_COUNTOF(fileName), ".bin");

	//	shaderPath /= fileName;

	//	//spdlog::info("Loading shader from: { }", shaderPath.string());

	//	bgfx::ShaderHandle handle = bgfx::createShader(loadMem(shaderPath.string()));

	//	return handle;
	//}

	//bgfx::ShaderHandle loadShader(const std::string_view& _name) {
	//	return loadShader_t(_name);
	//}

	/// <summary>
	/// New way of loading shader program
	/// </summary>
	static bgfx::ProgramHandle loadProgram(bx::FileReaderI* _reader, std::initializer_list<bx::StringView> _shaderNames) {
		if (_shaderNames.size() < 0 || _shaderNames.size() > 2) {
			spdlog::error("Invalid number of shaders");
			return BGFX_INVALID_HANDLE;
		}
		auto itr = _shaderNames.begin();
		if (_shaderNames.size() == 1) {
			spdlog::info("Creating compute shader");
			bgfx::ShaderHandle csShader = loadShader(_reader, *itr);
			return bgfx::createProgram(csShader, true);
		}

		spdlog::info("Creating vertex and fragment shader");
		bgfx::ShaderHandle vsShader = loadShader(_reader, *itr);
		bgfx::ShaderHandle fsShader = loadShader(_reader, *(++itr));
		return bgfx::createProgram(vsShader, fsShader, true);
	}

	bgfx::ProgramHandle loadProgram(std::initializer_list<bx::StringView> _shaderNames) {
		return loadProgram(_fileReader, _shaderNames);
	}

	/// <summary>
	/// old way to load bgfx shader program
	/// </summary>
	/*static bgfx::ProgramHandle loadProgram_t(std::initializer_list<std::string_view> _shaderNames) {
		if (_shaderNames.size() < 0 || _shaderNames.size() > 2) {
			spdlog::error("Invalid number of shaders");
			return BGFX_INVALID_HANDLE;
		}

		auto itr = _shaderNames.begin();
		if (_shaderNames.size() == 1) {
			spdlog::info("Creating compute shader");
			bgfx::ShaderHandle csShader = loadShader(*itr);
			return bgfx::createProgram(csShader, true);
		}

		spdlog::info("Creating vertex and fragment shader");
		bgfx::ShaderHandle vsShader = loadShader(*itr);
		bgfx::ShaderHandle fsShader = loadShader(*(++itr));
		return bgfx::createProgram(vsShader, fsShader, true);
	}

	bgfx::ProgramHandle loadProgram(std::initializer_list<std::string_view> _shaderNames) {
		return loadProgram_t(_shaderNames);
	}*/

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

	bx::AllocatorI* getDefaultAllocator()
	{
		BX_PRAGMA_DIAGNOSTIC_PUSH();
		BX_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(4459); // warning C4459: declaration of 's_allocator' hides global declaration
		BX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC("-Wshadow");
		static bx::DefaultAllocator s_allocator;
		return &s_allocator;
		BX_PRAGMA_DIAGNOSTIC_POP();
	}

}

