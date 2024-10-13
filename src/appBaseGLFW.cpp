#include <appBaseGLFW.hpp>

namespace shift {
	AppBaseGLFW::AppBaseGLFW(const char* name, const char* description, const char* url) : 
		shift::AppBase(name, description, url), 
		_width(0), 
		_height(0), 
		_title(name) {}

	void AppBaseGLFW::init(int _argc, const char** _argv, uint32_t width, uint32_t height) {
		// Read the args from command line
		Args args(_argc, _argv);

		_width = width;
		_height = height;

		// init glfw
		if (!glfwInit()) {
			return;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		_window = glfwCreateWindow(_width, _height, _title, nullptr, nullptr);

		// init bgfx
		bgfx::Init init;
		init.type = args._type;
		init.vendorId = args._pciId;
		init.platformData.nwh = shift::glfwNativeWindowHandle(_window);
		init.platformData.ndt = shift::glfwNativeDisplayHandle();
		init.platformData.type = shift::getNativeWindowHandleType();
		init.resolution.width = _width;
		init.resolution.height = _height;
		init.resolution.reset = BGFX_RESET_VSYNC;
		bgfx::init(init);

		// enable debug text
		bgfx::setDebug(BGFX_DEBUG_NONE);


	}

	bool AppBaseGLFW::update() {
		return true;
	}

	int AppBaseGLFW::shutdown() {
		return 0;
	}
}