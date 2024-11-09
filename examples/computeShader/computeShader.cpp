#include <utils/common.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <appBaseGLFW.hpp>

/// data used for quad
struct quadPosTexCoord {
	float _x;
	float _y;

	float _u;
	float _v;

	static void init() {
		_layout
			.begin()
			.add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			.end();
	};

	static bgfx::VertexLayout _layout;
};

bgfx::VertexLayout quadPosTexCoord::_layout;

quadPosTexCoord quadVertices[] = {
	{-1.0f, -1.0f, 0.0f, 0.0f},
	{ 1.0f, -1.0f, 1.0f, 0.0f},
	{-1.0f,  1.0f, 0.0f, 1.0f},
	{ 1.0f,  1.0f, 1.0f, 1.0f},
};

const uint16_t quadIndices[] = {
    2, 1, 0,
    2, 3, 1,
};

/// ERROR: uint16_t is necessary, otherwise it won't display anything
//static const unsigned int quadIndices[] = {
//    0, 1, 2,
//    2, 3, 0,
//};

const uint32_t kThreadGroupUpdateSize = 512;
const uint32_t kMaxParticleCount = 32 * 1024;

class ExampleComputeShader : public shift::AppBaseGLFW {

    void init(int _argc, const char** _argv, uint32_t width, uint32_t height) override {
        shift::AppBaseGLFW::init(_argc, _argv, width, height);

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

            _test2 = bgfx::createUniform("test2", bgfx::UniformType::Vec4);

            // create quad program
            glm::vec4 test2 = glm::vec4(1.0f);
            bgfx::setUniform(_test2, &test2);
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

            _test = bgfx::createUniform("test", bgfx::UniformType::Vec4);

            // textures/images

            // create compute shaders
            glm::vec4 test = glm::vec4(1.0f);
            bgfx::setUniform(_test, &test);
            _csProgramWithBuffer = shift::loadProgram({ "buffer_cs.sc" });

            // why dispatch at init?
            // because in the compute shader example, it try to init the data with compute shader
            // check example Nbody
            //bgfx::dispatch(0, _csProgramWithBuffer, SHIFT_DEFAULT_WIDTH * SHIFT_DEFAULT_WIDTH, 1, 1);

            //_csProgramWithImage = shift::loadProgram({ "image_cs.sc" });
        }

    }

    bool update() override {
        if (!glfwWindowShouldClose(_window)) {
            glfwSwapBuffers(_window);
            glfwPollEvents();

            // TODO: rendering everything here
            /* Compute shader */
            if (_computeSupported) {
                bgfx::setBuffer(0, s_buffers, bgfx::Access::Write);
                // specified the work group count
                glm::vec4 test = glm::vec4(1.0f);
                bgfx::setUniform(_test, &test, 1);
                bgfx::dispatch(0, _csProgramWithBuffer, kMaxParticleCount / kThreadGroupUpdateSize, 1, 1);
            }

            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(70.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            glm::mat4 proj = glm::perspective(glm::radians(60.0f), float(getWidth()) / getHeight(), 0.1f, 100.0f);
            bgfx::setViewTransform(0, &view[0][0], &proj[0][0]);
            bgfx::setViewRect(0, 0, 0, uint16_t(getWidth()), uint16_t(getHeight()));

            /* Quad rendering */
            glm::vec4 test = glm::vec4(1.0f);
            bgfx::setUniform(_test2, &test, 1);
            bgfx::setTransform(&model[0][0]);
            bgfx::setVertexBuffer(0, _vbhQuad);
            bgfx::setIndexBuffer(_ibhQuad);
            bgfx::setBuffer(0, s_buffers, bgfx::Access::Read);
            // chech https://bkaradzic.github.io/bgfx/bgfx.html#_CPPv4N4bgfx7Encoder8setStateE8uint64_t8uint32_t
            //bgfx::setState(BGFX_STATE_DEFAULT);
            bgfx::setState(0
                //| BGFX_STATE_PT_LINESTRIP
                | BGFX_STATE_WRITE_RGB
                | BGFX_STATE_BLEND_ADD
                | BGFX_STATE_DEPTH_TEST_ALWAYS
            );
            //bgfx::setState(BGFX_STATE_CULL_CW);
            bgfx::submit(0, _quadProgram);
    
            bgfx::frame();
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
            bgfx::destroy(_vbhQuad);
            bgfx::destroy(_ibhQuad);
            bgfx::destroy(s_buffers);
            bgfx::destroy(_test);
            bgfx::destroy(_test2);
            //bgfx::destroy(_imgCS);
        }
        bgfx::destroy(_quadProgram);
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

    bgfx::ProgramHandle _csProgramWithBuffer;
    bgfx::ProgramHandle _csProgramWithImage;
    bgfx::ProgramHandle _quadProgram;
    bgfx::VertexBufferHandle _vbhQuad;
    bgfx::IndexBufferHandle _ibhQuad;
    bgfx::DynamicVertexBufferHandle s_buffers;
    bgfx::TextureHandle _imgCS;
    bgfx::UniformHandle _test;
    bgfx::UniformHandle _test2;
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
