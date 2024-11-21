#include "utils/imgui.hpp"
#include <appBaseGLFW.hpp>
#include <utils/common.hpp>

double x, y;

class ExampleHelloBGFX final : public shift::AppBaseGLFW
{

    void init(int _argc, const char **_argv, uint32_t width, uint32_t height) override
    {
        shift::AppBaseGLFW::init(_argc, _argv, width, height);

        // init the buffer data herer
        spdlog::info("init func call by cube");

        // need to track the gleq again here
        // otherwise it wont capture the input event
        gleqTrackWindow(_window);
    }

    bool update() override
    {
        if (!glfwWindowShouldClose(_window))
        {
            glfwSwapBuffers(_window);
            glfwPollEvents();

            // render the imgui frame
            // imguiBeginFrame(_event.pos.x, _event.pos.y,
            //                ((_event.mouse.button == GLFW_MOUSE_BUTTON_LEFT ? IMGUI_MBUT_LEFT : 0) |
            //                 (_event.mouse.button == GLFW_MOUSE_BUTTON_RIGHT ? IMGUI_MBUT_RIGHT : 0) |
            //                 (_event.mouse.button == GLFW_MOUSE_BUTTON_MIDDLE ? IMGUI_MBUT_MID : 0)),
            //                int32_t(_event.scroll.x), uint16_t(SHIFT_DEFAULT_WIDTH), uint16_t(SHIFT_DEFAULT_HEIGHT));

            imguiBeginFrame(x, y, 1, int32_t(_event.scroll.x), uint16_t(SHIFT_DEFAULT_WIDTH),
                            uint16_t(SHIFT_DEFAULT_HEIGHT));

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
                std::cout << "hello" << std::endl;
            }

            bgfx::frame();

            // handle inpute event
            while (gleqNextEvent(&_event))
            {
                switch (_event.type)
                {
                case GLEQ_CURSOR_MOVED:
                    // let imgui window know that the cursor in now on its area
                    glfwGetCursorPos(_window, &_event.pos.x, &_event.pos.y);
                    std::cout << _event.pos.x << std::endl;
                    x = _event.pos.x;
                    y = _event.pos.y;
                    break;

                case GLEQ_BUTTON_PRESSED:
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
};

int main(int _argc, const char **_argv)
{

    ExampleHelloBGFX helloBgfx{"Example helloBgfx", "A Simple bgfx app with imgui",
                               "https://github.com/jintaoyugithub/Shift/tree/main/examples/helloBgfx"};
    helloBgfx.run(_argc, _argv);

    return 0;
}
