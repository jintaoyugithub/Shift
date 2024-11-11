#include <utils/common.hpp>
#include <appBaseGLFW.hpp>

struct PosTexcoordVertex {
    // position
    float _x;
    float _y;
    float _z;
    // texture coord
    int16_t _u;
    int16_t _v;

    static void init() {
        _layout
            .begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Int16, true, true)
            .end();
    }
    static bgfx::VertexLayout _layout;
};

bgfx::VertexLayout PosTexcoordVertex::_layout;

static PosTexcoordVertex quadVertices[] = {
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

class ExampleQuad : public shift::AppBaseGLFW {
    void init(int _argc, const char** _argv, uint32_t width, uint32_t height) override {
        shift::AppBaseGLFW::init(_argc, _argv, width, height);

        bgfx::setViewClear(0
            , BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
            , 0x303030ff
            , 1.0f
            , 0
        );

        PosTexcoordVertex::init();

        _vbh = bgfx::createVertexBuffer(bgfx::makeRef(quadVertices, sizeof(quadVertices)), PosTexcoordVertex::_layout);
        _ibh = bgfx::createIndexBuffer(bgfx::makeRef(quadIndices, sizeof(quadIndices)));
        _program = shift::loadProgram({ "quad_vs.sc", "quad_fs.sc" });
    }

    void shutdown() override {
        bgfx::destroy(_vbh);
        bgfx::destroy(_ibh);
        bgfx::destroy(_program);
    }

    bool update() override {
        // Set up all matrices
        glm::mat4 model = glm::identity<glm::mat4>();
        model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0, 0.7, 0.2));
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 proj = glm::perspective(glm::radians(60.0f), float(getWidth()) / getHeight(), 0.1f, 100.0f);
        bgfx::setViewTransform(0, &view[0][0], &proj[0][0]);
        bgfx::setViewRect(0, 0, 0, uint16_t(getWidth()), uint16_t(getHeight()));
        bgfx::touch(0);

        if(!glfwWindowShouldClose(_window)) {
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

public:
    ExampleQuad(const char* name, const char* description, const char* url) :
        shift::AppBaseGLFW(name, description, url) {}
    ~ExampleQuad() { shutdown(); };

    void run(int _argc, const char** _argv) override {
        AppBaseGLFW::run(_argc, _argv);
    }

private:
    bgfx::ProgramHandle _program;
    bgfx::VertexBufferHandle _vbh;
    bgfx::IndexBufferHandle _ibh;
};

int main(int _argc, const char** _argv) {
    ExampleQuad quad{
        "Example Quad",
        "Rendering a quad",
        "https://github.com/jintaoyugithub/Shift/tree/main/examples/quad"
    };
    quad.run(_argc, _argv);

    return 0;
}