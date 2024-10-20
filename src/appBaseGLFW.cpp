#include <appBaseGLFW.hpp>

namespace shift {
	AppBaseGLFW::AppBaseGLFW(const char* name, const char* description, const char* url) : 
		shift::AppBase(name, description, url), 
		_window(nullptr),
		_width(SHIFT_DEFAULT_WIDTH), 
		_height(SHIFT_DEFAULT_HEIGHT), 
		_title(name) {}

	AppBaseGLFW::~AppBaseGLFW() {
		shutdown();
	}

	bool AppBaseGLFW::bgfxInit(int _argc, const char ** _argv) {
		// Read the args from command line
		Args args(_argc, _argv);

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

		// don't forget to call this func otherwise it won't render anything
        bgfx::setViewClear(0
            , BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
            , 0x303030ff
            , 1.0f
            , 0
        );

		// enable debug text
		bgfx::setDebug(BGFX_DEBUG_TEXT);

		// set view port
		bgfx::setViewRect(0, 0, 0, _width, _height);
		bgfx::touch(0);

		return true;
	}

	bool AppBaseGLFW::windowInit(uint32_t width, uint32_t height) {
		if (width != 0 && height != 0) {
			_width = width;
			_height = height;
		}

		// init glfw
		if (!glfwInit()) {
			spdlog::error("FAIL TO INIT GLFW");
			return false;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		_window = glfwCreateWindow(_width, _height, _title, nullptr, nullptr);

		if (!_window) {
			spdlog::error("FAIL TO CREATE A WINDOW");
			return false;
		}

		return true;
	}

	bool AppBaseGLFW::uiInit() {
		return true;
	}

	void AppBaseGLFW::init(int _argc, const char** _argv, uint32_t width, uint32_t height) {
		spdlog::info("Init func call by AppBaseGLFW");
		// init windows
		if (!windowInit(width, height)) {
			spdlog::error("FAIL TO INIT WINDOW");
		}

		// init bgfx
		if (!bgfxInit(_argc, _argv)) {
			spdlog::error("FAIL TO INIT BGFX");
		}

		if (!uiInit()) {
			spdlog::error("FAIL TO INIT UI");
		}

	}

	bool AppBaseGLFW::update() {
		return true;
	}

	void AppBaseGLFW::shutdown() {
		spdlog::info("Shutdown func call by AppBaseGLFW");
		bgfx::shutdown();
		glfwDestroyWindow(_window);
		glfwTerminate();
	}

	void AppBaseGLFW::run(int _argc, const char** _argv) {
		spdlog::info("Run func call by AppBaseGLFW");

		// init all resources
		init(_argc, _argv, getWidth(), getHeight());

		while (update()) {

		}
	}
}