#include <appBaseGLFW.hpp>
#include <memory>
#include <utils/common.hpp>

#include "GLFW/glfw3.h"
#include "bgfx/bgfx.h"
#include "bx/string.h"
#include "gleq.hpp"
#include "velocityField.hpp"

enum ViewportType
{
    Velocity,
    Density,
    Boundary,
};

struct quadPosTexCoord
{
    float _x;
    float _y;
    float _z;

    int16_t _u;
    int16_t _v;

    static void init()
    {
        _layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Int16, true, true)
            .end();
    };

    static bgfx::VertexLayout _layout;
};

bgfx::VertexLayout quadPosTexCoord::_layout;

static quadPosTexCoord quadVertices[] = {
    // left corner, right corner, top right, top left
    {-1.0, -1.0, 0.0, 0, 0},
    {1.0, -1.0, 0.0, 0x7fff, 0},
    {1.0, 1.0, 0.0, 0x7fff, 0x7fff},
    {-1.0, 1.0, 0.0, 0, 0x7fff},
};

static const uint16_t quadIndices[] = {
    0, 1, 2, 2, 3, 0,
};

/* Global Variables */
float lastFrame = 0.0f;
double lastMousePosX = 0.0f;
double lastMousePosY = 0.0f;
bool isPressed = false;

bool densityEnable = false;
bool velocityAdvectEnable = false;
bool velocityProjectEnable = false;

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

            // init vertex layout
            quadPosTexCoord::init();

            _vbhQuad =
                bgfx::createVertexBuffer(bgfx::makeRef(quadVertices, sizeof(quadVertices)), quadPosTexCoord::_layout);

            _ibhQuad = bgfx::createIndexBuffer(bgfx::makeRef(quadIndices, sizeof(quadIndices)));

            _quadProgram = shift::loadProgram({"quad_vs.sc", "quad_fs.sc"});

            velocity = new VelocityField(getWidth(), getHeight(), 0.02f, 75.0f);

            velocity->dispatch(ProgramType::reset, 0);
        }
    }

    void shutdown() override
    {
        if (_computeSupported)
        {
            bgfx::destroy(_quadProgram);
            bgfx::destroy(_vbhQuad);
            bgfx::destroy(_ibhQuad);
        }
    }

    bool update() override
    {
        if (!glfwWindowShouldClose(_window))
        {
            glfwSwapBuffers(_window);
            glfwPollEvents();

            /* Update uniforms */
            float curFrame = glfwGetTime();
            float deltaTime = curFrame - lastFrame;
            lastFrame = curFrame;

            // std::cout << "FPS: " << 1 / _uParams.deltaTime << std::endl;

            /* Quad rendering */
            bgfx::setVertexBuffer(0, _vbhQuad);
            bgfx::setIndexBuffer(_ibhQuad);
            bgfx::setBuffer(0, velocity->getBufferHandle(BufferType::isFluid), bgfx::Access::Read);
            bgfx::setBuffer(1, velocity->getBufferHandle(BufferType::curVelX), bgfx::Access::Read);
            bgfx::setBuffer(2, velocity->getBufferHandle(BufferType::curVelY), bgfx::Access::Read);
            // check https://bkaradzic.github.io/bgfx/bgfx.html#_CPPv4N4bgfx7Encoder8setStateE8uint64_t8uint32_t
            bgfx::setState(BGFX_STATE_DEFAULT);
            bgfx::submit(0, _quadProgram);

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

                        // calculate the velocity dir
                        glm::vec2 velDir = glm::vec2(x - lastMousePosX, y - lastMousePosY);
                        velDir = glm::normalize(velDir);

                        // update uniforms
                        velocity->updateUniforms(UniformType::mousePosX, x);
                        velocity->updateUniforms(UniformType::mousePosY, y);
                        velocity->updateUniforms(UniformType::mouseVelX, velDir.x);
                        // the origin is in the top left
                        velocity->updateUniforms(UniformType::mouseVelY, -velDir.y);

                        // for calculating the velocity dir of the mouse
                        lastMousePosX = _event.pos.x;
                        lastMousePosY = _event.pos.y;

                        // dispatch shader
                        velocity->dispatch(ProgramType::addSource, 0);

                        // velocity->dispatch(ProgramType::project, 0);

                        // Debug info
                        // std::cout << velDir.x << " " << velDir.y << std::endl;
                    }
                    break;
                }

                case GLEQ_BUTTON_RELEASED:
                    std::cout << "left button released" << std::endl;
                    isPressed = false;
                    velocity->dispatch(ProgramType::project, 0);
                    break;

                case GLEQ_KEY_PRESSED:
                    if (_event.keyboard.key == GLFW_KEY_R)
                    {
                        std::cout << "Reset the program" << std::endl;
                        velocity->dispatch(ProgramType::reset, 0);
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
        : shift::AppBaseGLFW(name, description, url), velocity(nullptr)
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
    bool _computeSupported;
    bool _indirectSupported;

    bgfx::ProgramHandle _quadProgram;
    bgfx::VertexBufferHandle _vbhQuad;
    bgfx::IndexBufferHandle _ibhQuad;

    bgfx::DynamicVertexBufferHandle _prevDensityField;
    bgfx::DynamicVertexBufferHandle _curDensityField;

    VelocityField *velocity;
};

int main(int _argc, const char **_argv)
{
    ExampleFluidSim fluidSim{"Fluid simulation example", "A Example of Eulerian fluid simulation with compute shader",
                             "https://github.com/jintaoyugithub/Shift/tree/main/examples/fluidSim"};
    fluidSim.run(_argc, _argv);

    return 0;
}
