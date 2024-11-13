#pragma once

#include <utils/common.hpp>
#include "appBase.hpp"

namespace shift {
	class AppBaseGLFW : public shift::AppBase {
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