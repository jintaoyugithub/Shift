#pragma once

#include <utils/common.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
	{ 1.0f,  1.0f, 1.0f, 1.0f},
	{-1.0f,  1.0f, 0.0f, 1.0f},
	{-1.0f, -1.0f, 0.0f, 0.0f},
	{ 1.0f, -1.0f, 1.0f, 0.0f},
};

const uint16_t quadIndices[] = {
    0, 1, 2,
    2, 3, 0,
};

/// ERROR: uint16_t is necessary, otherwise it won't display anything
//static const unsigned int quadIndices[] = {
//    0, 1, 2,
//    2, 3, 0,
//};

class ExampleComputeShader : public shift::AppBaseGLFW {

    void init(int _argc, const char** _argv, uint32_t width, uint32_t height) override {
        shift::AppBaseGLFW::init(_argc, _argv, width, height);

        /* Data required by Quad Shader*/
        quadPosTexCoord::init();

        _vbhQuad = bgfx::createVertexBuffer(
            bgfx::makeRef(quadVertices, sizeof(quadVertices)),
            quadPosTexCoord::_layout
        );

        _ibhQuad = bgfx::createIndexBuffer(
            bgfx::makeRef(quadIndices, sizeof(quadIndices))
        );

        _quadProgram = shift::loadProgram({ "quad_vs.sc", "quad_fs.sc" });

        /* Check if compute shader is supported */
        const bgfx::Caps* caps = bgfx::getCaps();                      // this func return renderer capabilities
        _computeSupported = !!(caps->supported & BGFX_CAPS_COMPUTE);

        /* Check if indirect rendering is supported */
        _indirectSupported = !!(caps->supported & BGFX_CAPS_DRAW_INDIRECT);

        /* Data required by Compute Shader*/
        if (_computeSupported) {
            // buffers
            bgfx::VertexLayout dvbLayout;
            dvbLayout
                .begin()
                .add(bgfx::Attrib::TexCoord0, 4, bgfx::AttribType::Float)
                .end();
            _dvbhCS = bgfx::createDynamicVertexBuffer(1 << 15, dvbLayout, BGFX_BUFFER_COMPUTE_READ_WRITE);

            // textures/images
        }

        // create a compute shader program
        _csProgramWithBuffer = shift::loadProgram({ "buffer_cs.sc" });
        //_csProgramWithImage = shift::loadProgram({ "image_cs.sc" });
    }

    bool update() override {
        if (!glfwWindowShouldClose(_window)) {
            glfwSwapBuffers(_window);
            glfwPollEvents();

            // TODO: rendering everything here
            /* Compute shader */
            if (_computeSupported) {
                bgfx::setBuffer(0, _dvbhCS, bgfx::Access::Write);
                // specified the work group count
                bgfx::dispatch(0, _csProgramWithBuffer, 8,1,1);

                /* Quad rendering */
                bgfx::setVertexBuffer(0, _vbhQuad);
                bgfx::setIndexBuffer(_ibhQuad);
                bgfx::setBuffer(1, _dvbhCS, bgfx::Access::Read);
                // chech https://bkaradzic.github.io/bgfx/bgfx.html#_CPPv4N4bgfx7Encoder8setStateE8uint64_t8uint32_t
                bgfx::setState(BGFX_STATE_DEFAULT);
                bgfx::submit(0, _quadProgram);
    
                bgfx::frame();

                return true;
            }
        }

        return false;
    }

    void shutdown() override {
        spdlog::info("Shutdown func called by ExampleComputeShader");

        // clean all the programs and buffers
        bgfx::destroy(_csProgramWithBuffer);
        //bgfx::destroy(_csProgramWithImage);
        bgfx::destroy(_quadProgram);
        bgfx::destroy(_vbhQuad);
        bgfx::destroy(_ibhQuad);
        bgfx::destroy(_dvbhCS);
        //bgfx::destroy(_imgCS);
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
    bgfx::DynamicVertexBufferHandle _dvbhCS;
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
