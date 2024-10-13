#include <utils/common.hpp>
#include "appBase.hpp"

namespace shift {
	class AppBaseGLFW : public shift::AppBase {
		/* All the call back functions */
		static void keyCb(GLFWwindow* window, int key, int scancode, int action, int modes);
		static void charCb(GLFWwindow* window, unsigned int codepoint);
		static void charModsCb(GLFWwindow* window, unsigned int codepoint, int mods);
		static void mouseButtonCb(GLFWwindow* window, int button, int action, int modes);
		static void cursorPosCb(GLFWwindow* window, double xpos, double ypos);
		static void curosrEnterCb(GLFWwindow* window, int entered);
		static void scroolCb(GLFWwindow* window, double xoffset, double yoffset);
		static void dropCb(GLFWwindow* window, int count, const char ** paths);
		static void windowSizeCb(GLFWwindow* window, unsigned int width, unsigned int height);

	private:
		virtual bool bgfxInit(int _argc, const char ** _argv) final override;
		virtual bool windowInit(uint32_t width, uint32_t height) override;
		virtual bool uiInit() override;
		
	public:
		AppBaseGLFW(const char* name, const char* description, const char* url);
		~AppBaseGLFW() override;
		virtual void init(int _argc, const char** _argv, uint32_t width, uint32_t height) override;
		virtual bool update() override;
		virtual void shutdown() override;
		virtual void run(int _argc, const char** _argv) override;

		inline uint32_t getWidth() const { return _width; };
		inline uint32_t getHeight() const { return _height; };

	protected:
		GLFWwindow* _window;

	private:
		uint32_t _width;
		uint32_t _height;
		const char* _title;
	};

}