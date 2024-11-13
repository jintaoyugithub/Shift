#include <utils/common.hpp>
#include <appBaseGLFW.hpp>
#include <iostream>

#define GLEQ_IMPLEMENTATION
#define GLEQ_STATIC
#include <gleq/gleq.hpp>

/// data used for quad
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

const uint32_t kThreadGroupUpdateSize = 512;
const uint32_t kMaxParticleCount = 32 * 1024;

void mouseButtonCb(GLFWwindow* window, int button, int action, int modes) {
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        std::cout << "left button pressed" << std::endl;
    }
}

class ExampleComputeShader : public shift::AppBaseGLFW {

    void init(int _argc, const char** _argv, uint32_t width, uint32_t height) override {
        shift::AppBaseGLFW::init(_argc, _argv, width, height);

        // init the input event system
        gleqTrackWindow(_window);

        /* Check if compute shader is supported */
        const bgfx::Caps* caps = bgfx::getCaps();                      // this func return renderer capabilities
        _computeSupported  = !!(caps->supported & BGFX_CAPS_COMPUTE);

        /* Check if indirect rendering is supported */
        _indirectSupported = !!(caps->supported & BGFX_CAPS_DRAW_INDIRECT);

        if (_computeSupported) {
            /* Data required by Quad Shader*/
            quadPosTexCoord::init();

            _vbhQuad = bgfx::createVertexBuffer(
                bgfx::makeRef(quadVertices, sizeof(quadVertices)),
                quadPosTexCoord::_layout
            );

            _ibhQuad = bgfx::createIndexBuffer(
                bgfx::makeRef(quadIndices, sizeof(quadIndices))
            );

            // create quad program
            _quadProgram = shift::loadProgram({ "quad_vs.sc", "quad_fs.sc" });

            /* Data required by Compute Shader*/
            // buffers
            bgfx::VertexLayout dvbLayout;
            dvbLayout
                .begin()
                .add(bgfx::Attrib::TexCoord0, 4, bgfx::AttribType::Float)
                .end();
            // which means s_buffers will have the index range from 0 - (512*512)-1
            // have to be careful when using the global invocation id, it have to match the size of the buffer
            // otherwise it might be the reason cause debug break
            // note this is memory size
            s_buffers = bgfx::createDynamicVertexBuffer(1<<15, dvbLayout, BGFX_BUFFER_COMPUTE_READ_WRITE);

            // textures/images

            // create compute shaders
            _csProgramWithBuffer = shift::loadProgram({ "buffer_cs.sc" });
        }
    }

    bool update() override {
        if (!glfwWindowShouldClose(_window)) {
            glfwSwapBuffers(_window);
            glfwPollEvents();

            // TODO: rendering everything here
            /* Compute shader */
            bgfx::setBuffer(0, s_buffers, bgfx::Access::Write);
            // specified the work group count
            bgfx::dispatch(0, _csProgramWithBuffer, kMaxParticleCount / kThreadGroupUpdateSize, 1, 1);


            /* Quad rendering */
            bgfx::setVertexBuffer(0, _vbhQuad);
            bgfx::setIndexBuffer(_ibhQuad);
            bgfx::setBuffer(0, s_buffers, bgfx::Access::Read);
            // chech https://bkaradzic.github.io/bgfx/bgfx.html#_CPPv4N4bgfx7Encoder8setStateE8uint64_t8uint32_t
            bgfx::setState(BGFX_STATE_DEFAULT);
            bgfx::submit(0, _quadProgram);
    
            bgfx::frame();

            // check the input events
            while(gleqNextEvent(&_event)) {
                switch(_event.type) {
                    case GLEQ_BUTTON_PRESSED:
                        std::cout << _event.pos.x << _event.pos.y << std::endl;
                }
            }

            return true;
        }

        return false;
    }

    void shutdown() override {
        spdlog::info("Shutdown func called by ExampleComputeShader");

        // clean all the programs and buffers
        if (_computeSupported) {
            bgfx::destroy(_csProgramWithBuffer);
            //bgfx::destroy(_csProgramWithImage);
            bgfx::destroy(_quadProgram);
            bgfx::destroy(_vbhQuad);
            bgfx::destroy(_ibhQuad);
            bgfx::destroy(s_buffers);
            //bgfx::destroy(_imgCS);
        }
    }

public:
    ExampleComputeShader(const char* name, const char* description, const char* url)
        : shift::AppBaseGLFW(name, description, url) {}
    ~ExampleComputeShader() {
        shutdown();
    }

    void run(int _argc, const char** _argv) {
        shift::AppBaseGLFW::run(_argc, _argv);
    }

private:
    bool _computeSupported;
    bool _indirectSupported;

    GLEQevent _event;

    bgfx::ProgramHandle _csProgramWithBuffer;
    bgfx::ProgramHandle _csProgramWithImage;
    bgfx::ProgramHandle _quadProgram;
    bgfx::VertexBufferHandle _vbhQuad;
    bgfx::IndexBufferHandle _ibhQuad;
    bgfx::DynamicVertexBufferHandle s_buffers;
    bgfx::TextureHandle _imgCS;
};

int main(int _argc, const char** _argv) {
    ExampleComputeShader csExample {
        "Compute shader example",
        "A example shows how to use compute shader with Shift",
        "https://github.com/jintaoyugithub/Shift/tree/main/examples/computeShader"
    };
    csExample.run(_argc, _argv);

    return 0;
}
