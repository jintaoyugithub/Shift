#include <appBaseGLFW.hpp>
#include <memory>
#include <utils/common.hpp>

#include "GLFW/glfw3.h"
#include "bgfx/bgfx.h"
#include "bx/string.h"
#include "fluidSimUtils.hpp"
#include "gleq.hpp"
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
bool EnableAdvect = true;
bool EnableProject = true;
bool DebugDispDiv = false;
bool DebugDispProject = false;
bool DebugDispAdvect = false;
float elapsed = 0;

float deltaX = 0.1;
float deltaY = 0.1;
float deltaZ = 0.1;

struct testSphere
{
    // center pos
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    float radius = 1.0f;
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
            velocityCube = new VelocityFieldCube(getWidth(), getHeight(), 512.0f);

            velocityGrid->dispatch(ProgramType::Reset, 0);

            // init the inter pos uniform with sphere
            velocityCube->updateUniforms(UniformType::InterPosX, sphere.x);
            velocityCube->updateUniforms(UniformType::InterPosY, sphere.y);
            velocityCube->updateUniforms(UniformType::InterPosZ, sphere.z);
            velocityCube->updateUniforms(UniformType::Radius, sphere.radius);
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
            elapsed += (curFrame - lastFrame);
            lastFrame = curFrame;

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
                // test cube rendering
                // set all the matrices
                glm::mat4 model = glm::identity<glm::mat4>();
                model = glm::rotate(model, glm::radians(15.0f * elapsed), glm::vec3(1.0, 0.0, 0.0));
                glm::mat4 view =
                    glm::lookAt(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                glm::mat4 proj = glm::perspective(glm::radians(60.0f), float(getWidth()) / getHeight(), 0.1f, 100.0f);

                bgfx::setTransform(&model[0][0]);
                bgfx::setViewTransform(0, &view[0][0], &proj[0][0]);

                // set viewport
                bgfx::setViewRect(0, 0, 0, uint16_t(getWidth()), uint16_t(getHeight()));
                velocityCube->RenderBoundBox();
            }

            bgfx::frame();

            // check the input events
            while (gleqNextEvent(&_event))
            {
                switch (_event.type)
                {
                case GLEQ_BUTTON_PRESSED:
                    std::cout << "left button pressed" << std::endl;
                    isPressed = true;
                    break;

                case GLEQ_CURSOR_MOVED: {
                    if (isPressed)
                    {
                        // set up uniforms
                        double x, y;
                        glfwGetCursorPos(_window, &x, &y);

                        // calculate the velocityGrid dir
                        glm::vec2 velDir = glm::vec2(x - lastMousePosX, y - lastMousePosY);
                        velDir = glm::normalize(velDir);

                        // update uniforms
                        velocityGrid->updateUniforms(UniformType::InterPosX, x);
                        velocityGrid->updateUniforms(UniformType::InterPosY, y);
                        velocityGrid->updateUniforms(UniformType::InterVelX, velDir.x);
                        //    the origin is in the top left
                        velocityGrid->updateUniforms(UniformType::InterVelY, -velDir.y);

                        // for calculating the velocity dir of the mouse
                        lastMousePosX = _event.pos.x;
                        lastMousePosY = _event.pos.y;

                        // dispatch shader
                        velocityGrid->dispatch(ProgramType::AddSource, 0);

                        //  Debug info
                        // std::cout << velDir.x << " " << velDir.y << std::endl;
                    }
                    break;
                }

                case GLEQ_BUTTON_RELEASED:
                    std::cout << "left button released" << std::endl;
                    isPressed = false;
                    break;

                case GLEQ_KEY_PRESSED:
                    if (_event.keyboard.key == GLFW_KEY_R)
                    {
                        std::cout << "Reset the program" << std::endl;
                        velocityGrid->dispatch(ProgramType::Reset, 0);
                        // EnableAdvect = false;
                        // EnableProject = false;
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

                    break;
                case GLEQ_KEY_REPEATED:
                    // control the sphere
                    if (_event.keyboard.key == GLFW_KEY_UP)
                    {
                        sphere.z -= deltaZ;
                        std::cout << sphere.x << " " << sphere.y << " " << sphere.z << std::endl;
                        velocityCube->updateUniforms(UniformType::InterPosZ, sphere.z);
                    }
                    if (_event.keyboard.key == GLFW_KEY_DOWN)
                    {
                        sphere.z += deltaZ;
                        std::cout << sphere.x << " " << sphere.y << " " << sphere.z << std::endl;
                        velocityCube->updateUniforms(UniformType::InterPosZ, sphere.z);
                    }
                    if (_event.keyboard.key == GLFW_KEY_LEFT)
                    {
                        sphere.x -= deltaX;
                        std::cout << sphere.x << " " << sphere.y << " " << sphere.z << std::endl;
                        velocityCube->updateUniforms(UniformType::InterPosX, sphere.x);
                    }
                    if (_event.keyboard.key == GLFW_KEY_RIGHT)
                    {
                        sphere.x += deltaX;
                        std::cout << sphere.x << " " << sphere.y << " " << sphere.z << std::endl;
                        velocityCube->updateUniforms(UniformType::InterPosX, sphere.x);
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
    testSphere sphere;

    bool _computeSupported;
    bool _indirectSupported;

    bgfx::DynamicVertexBufferHandle _prevDensityField;
    bgfx::DynamicVertexBufferHandle _curDensityField;

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
