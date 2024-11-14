#include <utils/common.hpp>
#include <appBaseGLFW.hpp>

// size of the area whcih perform the fluid sim
const uint16_t size = 254;

struct Mouse {
    double x;
    double y;
    bool isPressed = false;
};

struct quadPosTexCoord {
	float _x;
	float _y;
    float _z;

	int16_t _u;
	int16_t _v;

	static void init() {
		_layout
			.begin()
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
    { 1.0, -1.0, 0.0, 0x7fff, 0},
    { 1.0,  1.0, 0.0, 0x7fff, 0x7fff},
    {-1.0,  1.0, 0.0, 0, 0x7fff},
};

static const uint16_t quadIndices[] = {
    0, 1, 2,
    2, 3, 0,
};

class ExampleFluidSim : public shift::AppBaseGLFW {
    void init(int _argc, const char** _argv, uint32_t width, uint32_t height) override {
        shift::AppBaseGLFW::init(_argc, _argv, width, height);

        // init input event handle system
        gleqTrackWindow(_window);

         /* Check if compute shader is supported */
        const bgfx::Caps* caps = bgfx::getCaps();                    
        _computeSupported  = !!(caps->supported & BGFX_CAPS_COMPUTE);

        /* Check if indirect rendering is supported */
        _indirectSupported = !!(caps->supported & BGFX_CAPS_DRAW_INDIRECT);

        if(_computeSupported) {
            // init vertex layout
            quadPosTexCoord::init();

            _vbhQuad = bgfx::createVertexBuffer(
                bgfx::makeRef(quadVertices, sizeof(quadVertices)),
                quadPosTexCoord::_layout
            );

            _ibhQuad = bgfx::createIndexBuffer(
                bgfx::makeRef(quadIndices, sizeof(quadIndices))
            );

            _quadProgram = shift::loadProgram({"quad_vs.sc", "quad_fs.sc"});


            /* Init all the buffers that the compute shader will use */
            // 1. dynamic buffers
            bgfx::VertexLayout densityLayout;
            densityLayout
                .begin()
                .add(bgfx::Attrib::TexCoord0, 1, bgfx::AttribType::Float)
                .end();

            _prevDensityField = bgfx::createDynamicVertexBuffer(
                // size + 2 because we take boundary in
                getHeight() * getWidth(),                        // mem size, not necessary to include sizeof cause you already specify it in the layout
                densityLayout,                                   // vertex fluidBufferLayout
                BGFX_BUFFER_COMPUTE_READ_WRITE                   // buffer access
            );

            _curDensityField = bgfx::createDynamicVertexBuffer(
                getHeight() * getWidth(),
                densityLayout,
                BGFX_BUFFER_COMPUTE_READ_WRITE                   
            );

            bgfx::VertexLayout velocityLayout;
            velocityLayout
                .begin()
                .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
                .end();

            _prevVelocityField = bgfx::createDynamicVertexBuffer(
                getHeight() * getWidth(),
                velocityLayout,                                          
                BGFX_BUFFER_COMPUTE_READ_WRITE                   
            );

            _curVelocityField = bgfx::createDynamicVertexBuffer(
                getHeight() * getWidth(),
                velocityLayout,                                          
                BGFX_BUFFER_COMPUTE_READ_WRITE                   
            );

            // 2. uniforms
            _mousePos = bgfx::createUniform("mousePos", bgfx::UniformType::Vec4, 1);


            // imporve performance while put them together??
            _csInit = shift::loadProgram({"init_cs.sc"});
            _csDensityUpdate = shift::loadProgram({"density_update_cs.sc"});
            _csVelocityUpdate = shift::loadProgram({"velocity_update_cs.sc"});

            // buffers set up 
            bgfx::setBuffer(0, _prevDensityField, bgfx::Access::Write);
            bgfx::setBuffer(1, _prevVelocityField, bgfx::Access::Write);
            // dispatch the init compute shader
            bgfx::dispatch(0, _csInit, 512, 1, 1);
        }
    }

    void shutdown() override {
        if(_computeSupported) {
            bgfx::destroy(_csInit);
            bgfx::destroy(_csDensityUpdate);
            bgfx::destroy(_csVelocityUpdate);
            bgfx::destroy(_quadProgram);
            bgfx::destroy(_vbhQuad);
            bgfx::destroy(_ibhQuad);
            bgfx::destroy(_prevVelocityField);
            bgfx::destroy(_curVelocityField);
            bgfx::destroy(_prevDensityField);
            bgfx::destroy(_curDensityField);

            bgfx::destroy(_mousePos);
        }
    }

    bool update() override {
        if (!glfwWindowShouldClose(_window)) {
            glfwSwapBuffers(_window);
            glfwPollEvents();

            /* Compute shader dispatch */
            bgfx::setBuffer(0, _prevDensityField, bgfx::Access::Read);
            bgfx::setBuffer(1, _curDensityField, bgfx::Access::Write);
            glm::vec4 mousePos = glm::vec4(_mouse.x, _mouse.y, _mouse.isPressed, 0.0);
            bgfx::setUniform(_mousePos, &mousePos, 1);
            bgfx::dispatch(0, _csDensityUpdate, 512, 1, 1);

            /* Quad rendering */
            bgfx::setVertexBuffer(0, _vbhQuad);
            bgfx::setIndexBuffer(_ibhQuad);
            //bgfx::setBuffer(0, _prevDensityField, bgfx::Access::Read);
            bgfx::setBuffer(0, _curDensityField, bgfx::Access::Read);
            // chech https://bkaradzic.github.io/bgfx/bgfx.html#_CPPv4N4bgfx7Encoder8setStateE8uint64_t8uint32_t
            bgfx::setState(BGFX_STATE_DEFAULT);
            bgfx::submit(0, _quadProgram);
    
            bgfx::frame();

            // check the input events
            while(gleqNextEvent(&_event)) {
                switch(_event.type) {
                    case GLEQ_BUTTON_PRESSED:
                        std::cout << "left button pressed" << std::endl;
                        _mouse.isPressed = true;
                        break;
                    case GLEQ_CURSOR_MOVED:
                        if(_mouse.isPressed) {
                            // set up uniforms
                            glfwGetCursorPos(_window, &_mouse.x, &_mouse.y);
                            std::cout << _mouse.x << " " << _mouse.y << std::endl;
                        }
                        break;
                    case GLEQ_BUTTON_RELEASED:
                        std::cout << "left button released" << std::endl;
                        _mouse.isPressed = false;
                        break;
                }

                gleqFreeEvent(&_event);
            }

            return true;
        }

        return false;
    }

public:
    ExampleFluidSim(const char* name, const char* description, const char* url) : shift::AppBaseGLFW(name, description, url) {}
    ~ExampleFluidSim() { shutdown(); };

    void run(int _argc, const char** _argv) override {
        shift::AppBaseGLFW::run(_argc, _argv);
    }

private:
    bool _computeSupported;
    bool _indirectSupported;

    GLEQevent _event;
    Mouse _mouse;

    bgfx::ProgramHandle _csInit;
    bgfx::ProgramHandle _csDensityUpdate;
    bgfx::ProgramHandle _csVelocityUpdate;
    bgfx::ProgramHandle _quadProgram;
    bgfx::VertexBufferHandle _vbhQuad;
    bgfx::IndexBufferHandle _ibhQuad;
    bgfx::UniformHandle _mousePos;
    bgfx::DynamicVertexBufferHandle _prevVelocityField;
    bgfx::DynamicVertexBufferHandle _curVelocityField;
    bgfx::DynamicVertexBufferHandle _prevDensityField;
    bgfx::DynamicVertexBufferHandle _curDensityField;
};

int main(int _argc, const char** _argv) {
    ExampleFluidSim fluidSim {
        "Fluid simulation example",
        "A Example of Eulerian fluid simulation with compute shader",
        "https://github.com/jintaoyugithub/Shift/tree/main/examples/fluidSim"
    };
    fluidSim.run(_argc, _argv);

    return 0;
}