#include "dear-imgui/imgui.h"
#include <appBaseGLFW.hpp>
#include <utils/common.hpp>
#include <winsock.h>

class ExampleHelloBGFX final : public shift::AppBaseGLFW
{

    void init(int _argc, const char **_argv, uint32_t width, uint32_t height) override
    {
        shift::AppBaseGLFW::init(_argc, _argv, width, height);

        // init the buffer data herer
        spdlog::info("init func call by cube");

        gleqTrackWindow(_window);

        imguiCreate();
    }

    bool update() override
    {
        if (!glfwWindowShouldClose(_window))
        {
            glfwSwapBuffers(_window);
            glfwPollEvents();

            // begin render imgui
            imguiBeginFrame(_mouseState._x, _mouseState._y,
                            (_mouseState._buttons[shift::MouseButton::Left] ? IMGUI_MBUT_LEFT : 0) |
                                (_mouseState._buttons[shift::MouseButton::Right] ? IMGUI_MBUT_RIGHT : 0) |
                                (_mouseState._buttons[shift::MouseButton::Middle] ? IMGUI_MBUT_MID : 0),
                            _mouseState._z, uint16_t(SHIFT_DEFAULT_WIDTH), uint16_t(SHIFT_DEFAULT_HEIGHT));

            // Set the imgui window position
            ImGui::SetNextWindowPos(ImVec2(getWidth() - getHeight() / 5.0f - 10.0f, 10.0f), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(getWidth() / 5.0f, getHeight() / 3.5f), ImGuiCond_FirstUseEver);
            ImGui::Begin("Settings", NULL, 0);

            ImGui::Checkbox("Hello Bgfx", &_helloBgfx);

            // dont forget to call end child
            ImGui::End();

            // end render imgui
            imguiEndFrame();

            // Set view 0 default viewport.
            bgfx::setViewRect(0, 0, 0, uint16_t(SHIFT_DEFAULT_WIDTH), uint16_t(SHIFT_DEFAULT_HEIGHT));
            // This dummy draw call is here to make sure that view 0 is cleared
            // if no other draw calls are submitted to view 0.
            bgfx::touch(0);

            if (!_helloBgfx)
            {
                std::cout << "hello bgfx" << std::endl;
            }

            bgfx::frame();

            // handle inpute event
            while (gleqNextEvent(&_event))
            {
                switch (_event.type)
                {
                case GLEQ_CURSOR_MOVED:
                    // let imgui window know that the cursor in now on its area
                    glfwGetCursorPos(_window, &_mouseState._x, &_mouseState._y);
                    break;

                case GLEQ_BUTTON_PRESSED:
                    _mouseState._buttons[shift::MouseButton::Left] = 1;
                    break;
                default:
                    //_mouseState._buttons[shift::MouseButton::Left] = 1;
                    break;
                }
            }

            return true;
        }

        return false;
    }

    void shutdown() override
    {
        // clean all the buffer data, shader and so on
        spdlog::info("Shutdown func call by helloBgfx");

        imguiDestroy();
    }

  public:
    ExampleHelloBGFX(const char *name, const char *description, const char *url)
        : shift::AppBaseGLFW(name, description, url)
    {
    }

    ~ExampleHelloBGFX()
    {
        shutdown();
    };

    void run(int _argc, const char **_argv) override
    {
        AppBaseGLFW::run(_argc, _argv);
    }

  private:
    bool _helloBgfx;
    GLEQevent _event;
};

int main(int _argc, const char **_argv)
{

    ExampleHelloBGFX helloBgfx{"Example Cube", "Rendering a Cube with Shift frmework",
                               "https://github.com/jintaoyugithub/Shift/tree/main/examples/cube"};
    helloBgfx.run(_argc, _argv);

    return 0;
}
