#include <utils/common.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <appBaseGLFW.hpp>

struct PosColorVertex {
    float _x;
    float _y;
    float _z;
    uint32_t _rgba;

    static void init() {
        _layout
            .begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();
    };

    static bgfx::VertexLayout _layout;
};

bgfx::VertexLayout PosColorVertex::_layout;

PosColorVertex cubeVertices[] = {
    {-1.0f,  1.0f,  1.0f, 0xff000000 },
    { 1.0f,  1.0f,  1.0f, 0xff0000ff },
    {-1.0f, -1.0f,  1.0f, 0xff00ff00 },
    { 1.0f, -1.0f,  1.0f, 0xff00ffff },
    {-1.0f,  1.0f, -1.0f, 0xffff0000 },
    { 1.0f,  1.0f, -1.0f, 0xffff00ff },
    {-1.0f, -1.0f, -1.0f, 0xffffff00 },
    { 1.0f, -1.0f, -1.0f, 0xffffffff },
};

static const uint16_t cubeTriList[] = { 
    2, 1, 0, 
    2, 3, 1,
    5, 6, 4,
    7, 6, 5,
    4, 2, 0,
    6, 2, 4,
    3, 5, 1,
    3, 7, 5,
    1, 4, 0,
    1, 5, 4,
    6, 3, 2,
    7, 3, 6
};

class ExampleCube final : public shift::AppBaseGLFW {

    void init(int _argc, const char** _argv, uint32_t width, uint32_t height) override {
        shift::AppBaseGLFW::init(_argc, _argv, width, height);

        // don't forget to call this func otherwise it won't render anything
        bgfx::setViewClear(0
            , BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
            , 0x303030ff
            , 1.0f
            , 0
        );

        // init the buffer data herer
        spdlog::info("init func call by cube");

        PosColorVertex::init();

        _vbh = bgfx::createVertexBuffer(bgfx::makeRef(cubeVertices, sizeof(cubeVertices)), PosColorVertex::_layout);
        _ibh = bgfx::createIndexBuffer(bgfx::makeRef(cubeTriList, sizeof(cubeTriList)));
        _program = shift::loadProgram({ "vs_cube.sc", "fs_cube.sc" });
    }

    bool update() override {
        // Set up all matrix 
        glm::mat4 model = glm::identity<glm::mat4>();
        model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0, 0.7, 0.2));
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 proj = glm::perspective(glm::radians(60.0f), float(getWidth()) / getHeight(), 0.1f, 100.0f);
        bgfx::setViewTransform(0, &view[0][0], &proj[0][0]);
        bgfx::setViewRect(0, 0, 0, uint16_t(getWidth()), uint16_t(getHeight()));
        bgfx::touch(0);


		if (!glfwWindowShouldClose(_window)) {
			glfwSwapBuffers(_window);
			glfwPollEvents();

            // TODO: rendering everything here
            bgfx::setTransform(&model[0][0]);
            bgfx::setVertexBuffer(0, _vbh);
            bgfx::setIndexBuffer(_ibh);
            bgfx::setState(BGFX_STATE_DEFAULT);
            bgfx::submit(0, _program);

            bgfx::frame();

			return true;
		}

		return false;
    }

    void shutdown() override {
        // clean all the buffer data, shader and so on 
        spdlog::info("Shutdown func call by cube");

        bgfx::destroy(_vbh);
        bgfx::destroy(_ibh);
    }


public:
    ExampleCube(const char* name, const char* description, const char* url) :
        shift::AppBaseGLFW(name, description, url) {}

    ~ExampleCube() { shutdown(); };

    void run(int _argc, const char** _argv) override {
        AppBaseGLFW::run(_argc, _argv);
    }

private:
    bgfx::ProgramHandle _program;
    bgfx::VertexBufferHandle _vbh;
    bgfx::IndexBufferHandle _ibh;
};

int main(int _argc, const char** _argv) {

    ExampleCube cube{ 
        "Example Cube", 
        "Rendering a Cube with Shift frmework", 
        "https://github.com/jintaoyugithub/Shift/tree/main/examples/cube"
    };
    cube.run(_argc, _argv);

    return 0;
}
