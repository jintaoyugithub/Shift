#include <appBaseGLFW.hpp>
#include <memory>
#include <stdio.h>
#include <utils/common.hpp>

#include "fluidSimUtils.hpp"
#include "velocityField.hpp"

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

            velocity = std::make_unique<VelocityField>(getWidth(), getHeight(), 0.02, 75);
        }
    }

    void shutdown() override
    {
        if (_computeSupported)
        {
            bgfx::destroy(_quadProgram);
            bgfx::destroy(_vbhQuad);
            bgfx::destroy(_ibhQuad);
            bgfx::destroy(_prevDensityField);
            bgfx::destroy(_curDensityField);
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

            if (velocityAdvectEnable)
            {
            }

            if (densityEnable)
            {
            }

            /* Quad rendering */
            bgfx::setVertexBuffer(0, _vbhQuad);
            bgfx::setIndexBuffer(_ibhQuad);
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
                    break;
                case GLEQ_CURSOR_MOVED:
                    // set up uniforms
                    double x, y;
                    glfwGetCursorPos(_window, &x, &y);

                    // glm::vec2 velocity = glm::vec2(xDir, yDir);
                    // velocity = glm::normalize(velocity);
                    // std::cout << velocity.x << " " << velocity.y << std::endl;

                    lastMousePosX = x;
                    lastMousePosY = y;
                    break;
                case GLEQ_BUTTON_RELEASED:
                    std::cout << "left button released" << std::endl;
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

    std::unique_ptr<VelocityField> velocity;

    bgfx::ProgramHandle _quadProgram;
    bgfx::VertexBufferHandle _vbhQuad;
    bgfx::IndexBufferHandle _ibhQuad;

    bgfx::DynamicVertexBufferHandle _prevDensityField;
    bgfx::DynamicVertexBufferHandle _curDensityField;
};

int main(int _argc, const char **_argv)
{
    ExampleFluidSim fluidSim{"Fluid simulation example", "A Example of Eulerian fluid simulation with compute shader",
                             "https://github.com/jintaoyugithub/Shift/tree/main/examples/fluidSim"};
    fluidSim.run(_argc, _argv);

    return 0;
}
