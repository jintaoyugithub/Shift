#include <appBaseGLFW.hpp>
#include <exception>
#include <memory>
#include <utils/common.hpp>

#include "GLFW/glfw3.h"
#include "bgfx/bgfx.h"
#include "bx/string.h"
#include "fluidSimUtils.hpp"
#include "gleq.hpp"
#include "spdlog/spdlog.h"
#include "tinystl/buffer.h"
#include "velocityField.hpp"
#include "velocityFieldCube.hpp"
#include "velocityFieldGrid.hpp"

/* Global Variables */
float lastFrame = 0.0f;
double lastMousePosX = 0.0f;
double lastMousePosY = 0.0f;
bool isPressed = false;

// for debug
bool EnableAdvect = false;
bool EnableProject = false;
bool DebugDispDiv = false;
bool DebugDispProject = false;
bool DebugDispAdvect = false;
float elapsed = 0;

float deltaX = 0.01;
float deltaY = 0.01;
float deltaZ = 0.01;

enum VelocityDir
{
    Left,
    Right,
    Up,
    Down,
    Random,
};

class ExampleFluidSim : public shift::AppBaseGLFW
{
    void init(int _argc, const char **_argv, uint32_t width, uint32_t height) override
    {
        shift::AppBaseGLFW::init(_argc, _argv, width, height);

        // init input event handle system
        gleqTrackWindow(_window);

        /* Check if compute shader is supported */
        const bgfx::Caps *caps = bgfx::getCaps();
        _computeSupported = !!(caps->supported & BGFX_CAPS_COMPUTE);

        /* Check if indirect rendering is supported */
        _indirectSupported = !!(caps->supported & BGFX_CAPS_DRAW_INDIRECT);

        if (_computeSupported)
        {
            velocityGrid = new VelocityFieldGrid(getWidth(), getHeight(), 1.0f);
            velocityGrid->updateUniforms(UniformType::Radius, 10.0f);

            velocityGrid->dispatch(ProgramType::Reset, 0);
        }
    }

    void shutdown() override
    {
    }

    bool update() override
    {
        if (!glfwWindowShouldClose(_window))
        {
            glfwSwapBuffers(_window);
            glfwPollEvents();

            /* Update uniforms */
            float curFrame = glfwGetTime();
            float deltaTime = (curFrame - lastFrame);
            elapsed += deltaTime;
            lastFrame = curFrame;

            _camera->OnUpdate(deltaTime);

            // std::cout << "FPS: " << 1 / deltaTime << std::endl;

            // dispatch advect compute shader
            if (EnableAdvect)
            {
                swap(velocityGrid->getBufferHandle(BufferType::PrevVelX),
                     velocityGrid->getBufferHandle(BufferType::CurVelX));
                swap(velocityGrid->getBufferHandle(BufferType::PrevVelY),
                     velocityGrid->getBufferHandle(BufferType::CurVelY));
                velocityGrid->dispatch(ProgramType::Advect, 0);
            }

            // dispatch project compute shader
            if (EnableProject)
            {
                velocityGrid->dispatch(ProgramType::Project, 0);
            }

            /* Quad rendering */
            if (DebugDispDiv)
            {
                velocityGrid->DispDiv(0);
            }
            else if (DebugDispAdvect)
            {
                velocityGrid->DispAdvect(0);
            }
            else if (DebugDispProject)
            {
                velocityGrid->DispProject(0);
            }
            else
            {
                // disap isFluid in a new small viewport
                // int debugWindowSize = 128;
                // bgfx::setViewRect(1, getWidth() - debugWindowSize, getHeight() - debugWindowSize, getWidth(),
                //                   getHeight());
                // velocityGrid->dispatch(ProgramType::RenderBoundary, 1);

                // test cube rendering
                // set all the matrices
                glm::mat4 model = glm::identity<glm::mat4>();
                model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));

                glm::mat4 view = _camera->GetProperties<glm::mat4>(shift::Property::ViewMat);
                glm::mat4 proj = _camera->GetProperties<glm::mat4>(shift::Property::ProjMat);
                // bgfx::setTransform(&model[0][0]);
                bgfx::setViewTransform(0, &view[0][0], &proj[0][0]);

                // set viewport
                bgfx::setViewRect(0, 0, 0, uint16_t(getWidth()), uint16_t(getHeight()));
                // velocityCube->RenderBoundBox();
                velocityGrid->dispatch(ProgramType::RenderBoundary, 0);
            }

            bgfx::frame();

            // check the input events
            while (gleqNextEvent(&_event))
            {
                switch (_event.type)
                {
                case GLEQ_BUTTON_PRESSED:
                    switch (_event.mouse.button)
                    {
                    case GLFW_MOUSE_BUTTON_LEFT:
                        std::cout << "left button pressed" << std::endl;
                        isPressed = true;
                        break;
                    case GLFW_MOUSE_BUTTON_RIGHT:
                        std::cout << "right button pressed" << std::endl;
                        isPressed = true;
                        velocityGrid->updateUniforms(UniformType::IsRightPressed, 1.0f);
                        break;
                    }
                    break;

                case GLEQ_CURSOR_MOVED: {
                    double x, y;
                    glfwGetCursorPos(_window, &x, &y);
                    if (isPressed)
                    {
                        velocityGrid->updateUniforms(UniformType::InterPosX, x);
                        velocityGrid->updateUniforms(UniformType::InterPosY,
                                                     velocityGrid->getUniforms(UniformType::SimResY) - y);
                        // set up uniforms
                        switch (dirs)
                        {
                        case Left:
                            velocityGrid->updateUniforms(UniformType::InterVelX, -1.0f);
                            velocityGrid->updateUniforms(UniformType::InterVelY, 0.0f);
                            break;
                        case Right:
                            velocityGrid->updateUniforms(UniformType::InterVelX, 1.0f);
                            velocityGrid->updateUniforms(UniformType::InterVelY, 0.0f);
                            break;
                        case Up:
                            velocityGrid->updateUniforms(UniformType::InterVelX, 0.0f);
                            velocityGrid->updateUniforms(UniformType::InterVelY, 1.0f);
                            break;
                        case Down:
                            velocityGrid->updateUniforms(UniformType::InterVelX, 0.0f);
                            velocityGrid->updateUniforms(UniformType::InterVelY, -1.0f);
                            break;
                        case Random:
                            // calculate the velocityGrid dir
                            glm::vec2 velDir = glm::vec2(x - lastMousePosX, y - lastMousePosY);
                            velDir = glm::normalize(velDir);

                            // update uniforms
                            velocityGrid->updateUniforms(UniformType::InterVelX, velDir.x);
                            // the origin is in the top left
                            velocityGrid->updateUniforms(UniformType::InterVelY, -velDir.y);
                            // velocityGrid->updateUniforms(UniformType::InterVelY, velDir.y);

                            lastMousePosX = x;
                            lastMousePosY = y;
                            break;
                        }

                        velocityGrid->dispatch(ProgramType::AddSource, 0);
                    }

                    break;
                }

                case GLEQ_BUTTON_RELEASED:
                    switch (_event.mouse.button)
                    {
                    case GLFW_MOUSE_BUTTON_LEFT:
                        std::cout << "left button released" << std::endl;
                        isPressed = false;
                        break;
                    case GLFW_MOUSE_BUTTON_RIGHT:
                        std::cout << "right button released" << std::endl;
                        isPressed = false;
                        velocityGrid->updateUniforms(UniformType::IsRightPressed, 0.0f);
                        break;
                    }
                    break;

                case GLEQ_KEY_PRESSED:
                    if (_event.keyboard.key == GLFW_KEY_R)
                    {
                        std::cout << "Reset the program" << std::endl;
                        velocityGrid->dispatch(ProgramType::Reset, 0);
                        EnableAdvect = false;
                        EnableProject = false;
                    }

                    if (_event.keyboard.key == GLFW_KEY_A)
                    {
                        EnableAdvect = true;
                    }

                    if (_event.keyboard.key == GLFW_KEY_P)
                    {
                        EnableProject = true;
                    }

                    if (_event.keyboard.key == GLFW_KEY_1)
                    {
                        DebugDispDiv = true;
                        DebugDispProject = false;
                        DebugDispAdvect = false;
                    }

                    if (_event.keyboard.key == GLFW_KEY_2)
                    {
                        DebugDispProject = true;
                        DebugDispDiv = false;
                        DebugDispAdvect = false;
                    }

                    if (_event.keyboard.key == GLFW_KEY_3)
                    {
                        DebugDispAdvect = true;
                        DebugDispProject = false;
                        DebugDispDiv = false;
                    }

                    if (_event.keyboard.key == GLFW_KEY_0)
                    {
                        DebugDispAdvect = false;
                        DebugDispProject = false;
                        DebugDispDiv = false;
                    }

                    if (_event.keyboard.key == GLFW_KEY_UP)
                    {
                        dirs = VelocityDir::Up;
                        spdlog::info("Current Velocity Dir: (0.0, 1.0)");
                    }

                    if (_event.keyboard.key == GLFW_KEY_DOWN)
                    {
                        dirs = VelocityDir::Down;
                        spdlog::info("Current Velocity Dir: (0.0, -1.0)");
                    }

                    if (_event.keyboard.key == GLFW_KEY_LEFT)
                    {
                        dirs = VelocityDir::Left;
                        spdlog::info("Current Velocity Dir: (-1.0, 0.0)");
                    }

                    if (_event.keyboard.key == GLFW_KEY_RIGHT)
                    {
                        dirs = VelocityDir::Right;
                        spdlog::info("Current Velocity Dir: (1.0, 0.0)");
                    }

                    if (_event.keyboard.key == GLFW_KEY_V)
                    {
                        dirs = VelocityDir::Random;
                        spdlog::info("Current Velocity Dir is based on mouse move dir");
                    }
                    break;

                case GLEQ_KEY_REPEATED:
                    if (_event.keyboard.key == GLFW_KEY_MINUS)
                    {
                        float curRadius = velocityGrid->getUniforms(UniformType::Radius);
                        if (curRadius > 0.0f)
                        {
                            curRadius -= 1.0f;
                        }
                        velocityGrid->updateUniforms(UniformType::Radius, curRadius);
                        spdlog::info("Current brush radius: {}", curRadius);
                    }

                    if (_event.keyboard.key == GLFW_KEY_EQUAL)
                    {
                        float curRadius = velocityGrid->getUniforms(UniformType::Radius);
                        curRadius += 1.0f;
                        velocityGrid->updateUniforms(UniformType::Radius, curRadius);
                        spdlog::info("Current brush radius: {}", curRadius);
                    }
                    break;

                case GLEQ_KEY_RELEASED:
                    break;
                }

                gleqFreeEvent(&_event);
            }

            return true;
        }
        return false;
    }

  public:
    ExampleFluidSim(const char *name, const char *description, const char *url)
        : shift::AppBaseGLFW(name, description, url), velocityGrid(nullptr), velocityCube(nullptr)
    {
    }

    ~ExampleFluidSim()
    {
        shutdown();
    };

    void run(int _argc, const char **_argv) override
    {
        shift::AppBaseGLFW::run(_argc, _argv);
    }

  private:
    VelocityDir dirs = VelocityDir::Random;

    bool _computeSupported;
    bool _indirectSupported;

    VelocityFieldGrid *velocityGrid;
    VelocityFieldCube *velocityCube;
};

int main(int _argc, const char **_argv)
{
    ExampleFluidSim fluidSim{"Fluid simulation example", "A Example of Eulerian fluid simulation with compute shader",
                             "https://github.com/jintaoyugithub/Shift/tree/main/examples/fluidSim"};
    fluidSim.run(_argc, _argv);

    return 0;
}
