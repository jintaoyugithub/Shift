#include "bgfx/bgfx.h"
#include "glm/geometric.hpp"
#include <appBaseGLFW.hpp>
#include <corecrt_math.h>
#include <ios>
#include <utils/common.hpp>

#include <algorithm>
#include <xlocale>

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

struct paramsData
{
    // for mouse input
    float x;
    float y;
    float xAcce;
    float yAcce;
    // true means velocity, false means density
    float state;

    // for buffer info
    // int32_t bufferWidth;
    float bufferWidth;
    float bufferHeight;
    float bufferResolution;

    // for equation parameters
    float deltaTime;
    float diff;
    float visc;
    float isMousePressed;
};

void initParamsData(paramsData *_data)
{
    _data->x = 3.40e+38; // avoid the origin problem of the addSource compute shader
    _data->y = 3.40e+38;
    _data->xAcce = 0.0f;
    _data->yAcce = 0.0f;
    _data->state = true;
    _data->bufferWidth = SHIFT_DEFAULT_WIDTH;
    _data->bufferHeight = SHIFT_DEFAULT_HEIGHT;
    _data->bufferResolution = SHIFT_DEFAULT_WIDTH * SHIFT_DEFAULT_HEIGHT;
    _data->deltaTime = 0.0f;
    _data->diff = 1.0e-7;
    _data->visc = 1.5f * 1.0e-7;
    _data->isMousePressed = false;
}

void swap(bgfx::DynamicVertexBufferHandle &prev, bgfx::DynamicVertexBufferHandle &cur)
{
    bgfx::DynamicVertexBufferHandle temp = cur;
    prev = cur;
    cur = temp;
}

float lastFrame = 0.0f;
double lastMousePosX = 0.0f;
double lastMousePosY = 0.0f;
const int kThreadGroupSizeX = 32;
const int kThreadGroupSizeY = 32;

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

            /* Init all the buffersthat the compute shader will use */
            // 1. dynamic buffers
            bgfx::VertexLayout densityLayout;
            densityLayout.begin().add(bgfx::Attrib::TexCoord0, 1, bgfx::AttribType::Float).end();

            _prevDensityField = bgfx::createDynamicVertexBuffer(
                // size + 2 because we take boundary in
                getHeight() * getWidth(),      // mem size, not necessary to include sizeof cause you
                                               // already specify it in the layout
                densityLayout,                 // vertex fluidBufferLayout
                BGFX_BUFFER_COMPUTE_READ_WRITE // buffer access
            );

            _curDensityField = bgfx::createDynamicVertexBuffer(getHeight() * getWidth(), densityLayout,
                                                               BGFX_BUFFER_COMPUTE_READ_WRITE);

            bgfx::VertexLayout velocityLayout;
            velocityLayout.begin().add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float).end();

            _prevVelocityField = bgfx::createDynamicVertexBuffer(getHeight() * getWidth(), velocityLayout,
                                                                 BGFX_BUFFER_COMPUTE_READ_WRITE);

            _curVelocityField = bgfx::createDynamicVertexBuffer(getHeight() * getWidth(), velocityLayout,
                                                                BGFX_BUFFER_COMPUTE_READ_WRITE);

            // 2. uniforms
            _uhParams = bgfx::createUniform("uParams", bgfx::UniformType::Vec4, 3);
            initParamsData(&_uParams);

            bufferSize = bgfx::createUniform("bufferSize", bgfx::UniformType::Vec4, 1);

            // imporve performance while put them together??
            _csInit = shift::loadProgram({"init_cs.sc"});

            _csAddSource = shift::loadProgram({"addSource_cs.sc"});
            _csDiffuse = shift::loadProgram({"diffuse_cs.sc"});
            _csAdvect = shift::loadProgram({"advect_cs.sc"});

            //_csVelocityProject = shift::loadProgram({"velocity_update_cs.sc"});

            // buffers set up
            bgfx::setBuffer(0, _prevDensityField, bgfx::Access::Write);
            bgfx::setBuffer(1, _prevVelocityField, bgfx::Access::Write);
            bgfx::setBuffer(2, _curVelocityField, bgfx::Access::Write);
            bgfx::setUniform(_uhParams, &_uParams, 3);
            // dispatch the init compute shader
            bgfx::dispatch(0, _csInit, _uParams.bufferWidth / kThreadGroupSizeX,
                           _uParams.bufferHeight / kThreadGroupSizeY, 1);
        }
    }

    void shutdown() override
    {
        if (_computeSupported)
        {
            bgfx::destroy(_csInit);
            bgfx::destroy(_csAddSource);
            bgfx::destroy(_csDiffuse);
            bgfx::destroy(_csAdvect);
            // bgfx::destroy(_csVelocityProject);
            bgfx::destroy(_quadProgram);
            bgfx::destroy(_vbhQuad);
            bgfx::destroy(_ibhQuad);
            bgfx::destroy(_prevVelocityField);
            bgfx::destroy(_curVelocityField);
            bgfx::destroy(_prevDensityField);
            bgfx::destroy(_curDensityField);
            bgfx::destroy(_uhParams);
            bgfx::destroy(bufferSize);
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
            _uParams.deltaTime = curFrame - lastFrame;
            lastFrame = curFrame;

            std::cout << "FPS: " << 1 / _uParams.deltaTime << std::endl;

            // Velocity compute shaders
            // add velocity
            bgfx::setBuffer(0, _prevDensityField, bgfx::Access::Read);
            bgfx::setBuffer(1, _curDensityField, bgfx::Access::ReadWrite);
            bgfx::setBuffer(2, _prevVelocityField, bgfx::Access::Read);
            bgfx::setBuffer(3, _curVelocityField, bgfx::Access::ReadWrite);
            bgfx::setUniform(_uhParams, &_uParams, 3);
            bgfx::dispatch(0, _csAddSource, _uParams.bufferWidth / kThreadGroupSizeX,
                           _uParams.bufferHeight / kThreadGroupSizeY, 1);

            swap(_prevVelocityField, _curVelocityField);
            swap(_prevDensityField, _curDensityField);

            bgfx::setBuffer(0, _prevDensityField, bgfx::Access::Read);
            bgfx::setBuffer(1, _curDensityField, bgfx::Access::ReadWrite);
            bgfx::setBuffer(2, _prevVelocityField, bgfx::Access::Read);
            bgfx::setBuffer(3, _curVelocityField, bgfx::Access::ReadWrite);
            bgfx::setUniform(_uhParams, &_uParams, 3);
            bgfx::dispatch(0, _csDiffuse, _uParams.bufferWidth / kThreadGroupSizeX,
                           _uParams.bufferHeight / kThreadGroupSizeY, 1);

            // need project here

            swap(_prevVelocityField, _curVelocityField);

            bgfx::setBuffer(0, _prevDensityField, bgfx::Access::Read);
            bgfx::setBuffer(1, _curDensityField, bgfx::Access::ReadWrite);
            bgfx::setBuffer(2, _prevVelocityField, bgfx::Access::Read);
            bgfx::setBuffer(3, _curVelocityField, bgfx::Access::ReadWrite);
            bgfx::setUniform(_uhParams, &_uParams, 3);
            bgfx::dispatch(0, _csAdvect, _uParams.bufferWidth / kThreadGroupSizeX,
                           _uParams.bufferHeight / kThreadGroupSizeY, 1);

            swap(_prevVelocityField, _curVelocityField);

            // need project here

            // set state to false to enable density calculation in the compute shader
            _uParams.state = false;

            /* Note:
             * should add source at the same time because of the input system
             */
            // Density compute shaders
            // dispatch add source compute shader
            // bgfx::setBuffer(0, _prevDensityField, bgfx::Access::Read);
            // bgfx::setBuffer(1, _curDensityField, bgfx::Access::ReadWrite);
            // bgfx::setBuffer(2, _prevVelocityField, bgfx::Access::Read);
            // bgfx::setBuffer(3, _curVelocityField, bgfx::Access::ReadWrite);
            // bgfx::setUniform(_uhParams, &_uParams, 3);
            // bgfx::dispatch(0, _csAddSource, _uParams.bufferWidth / kThreadGroupSizeX,
            //               _uParams.bufferHeight / kThreadGroupSizeY, 1);

            //// dont forget to swap the buffer
            //// std::swap(_prevDensityField, _curDensityField);

            // dispatch diffuse compute shader
            bgfx::setBuffer(0, _prevDensityField, bgfx::Access::Read);
            bgfx::setBuffer(1, _curDensityField, bgfx::Access::ReadWrite);
            bgfx::setBuffer(2, _prevVelocityField, bgfx::Access::Read);
            bgfx::setBuffer(3, _curVelocityField, bgfx::Access::ReadWrite);
            bgfx::setUniform(_uhParams, &_uParams, 3);
            bgfx::dispatch(0, _csDiffuse, _uParams.bufferWidth / kThreadGroupSizeX,
                           _uParams.bufferHeight / kThreadGroupSizeY, 1);

            // std::swap(_prevDensityField, _curDensityField);
            swap(_prevDensityField, _curDensityField);

            // dispatch advect compute shader
            bgfx::setBuffer(0, _prevDensityField, bgfx::Access::Read);
            bgfx::setBuffer(1, _curDensityField, bgfx::Access::ReadWrite);
            bgfx::setBuffer(2, _prevVelocityField, bgfx::Access::Read);
            bgfx::setBuffer(3, _curVelocityField, bgfx::Access::ReadWrite);
            bgfx::setUniform(_uhParams, &_uParams, 3);
            bgfx::dispatch(0, _csAdvect, _uParams.bufferWidth / kThreadGroupSizeX,
                           _uParams.bufferHeight / kThreadGroupSizeY, 1);

            _uParams.state = true;

            /* Quad rendering */
            bgfx::setVertexBuffer(0, _vbhQuad);
            bgfx::setIndexBuffer(_ibhQuad);
            bgfx::setBuffer(0, _curDensityField, bgfx::Access::Read);
            bgfx::setBuffer(1, _curVelocityField, bgfx::Access::Read);
            bgfx::setUniform(_uhParams, &_uParams, 3);
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
                    _uParams.isMousePressed = true;
                    break;
                case GLEQ_CURSOR_MOVED:
                    if (_uParams.isMousePressed)
                    {
                        // set up uniforms
                        double x, y;
                        glfwGetCursorPos(_window, &x, &y);
                        _uParams.x = x;
                        _uParams.y = y;
                        //_uParams.xAcce = std::clamp(lastMousePosX - x, -1.0, 1.0);
                        //_uParams.yAcce = std::clamp(lastMousePosY - y, -1.0, 1.0);
                        float xDir = (_uParams.x - lastMousePosX) / _uParams.deltaTime;
                        float yDir = (_uParams.y - lastMousePosY) / _uParams.deltaTime;

                        glm::vec2 velocity = glm::vec2(xDir, yDir);
                        velocity = glm::normalize(velocity);
                        std::cout << velocity.x << " " << velocity.y << std::endl;

                        _uParams.xAcce = velocity.x;
                        _uParams.yAcce = velocity.y;

                        lastMousePosX = x;
                        lastMousePosY = y;

                        // update the liftTime
                        //_uParams.liftTime -= _uParams.deltaTime;
                    }
                    break;
                case GLEQ_BUTTON_RELEASED:
                    std::cout << "left button released" << std::endl;
                    _uParams.isMousePressed = false;
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
        : shift::AppBaseGLFW(name, description, url)
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

    GLEQevent _event;
    paramsData _uParams;

    bgfx::ProgramHandle _csInit;

    // compute shader used for density calculation
    bgfx::ProgramHandle _csAddSource;
    bgfx::ProgramHandle _csDiffuse;
    bgfx::ProgramHandle _csAdvect;

    bgfx::ProgramHandle _csVelocityProject;

    bgfx::ProgramHandle _quadProgram;
    bgfx::VertexBufferHandle _vbhQuad;
    bgfx::IndexBufferHandle _ibhQuad;
    bgfx::UniformHandle _uhParams;
    bgfx::UniformHandle bufferSize;

    bgfx::DynamicVertexBufferHandle _prevVelocityField;
    bgfx::DynamicVertexBufferHandle _curVelocityField;

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
