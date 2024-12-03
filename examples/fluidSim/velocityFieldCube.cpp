#include "bgfx/bgfx.h"
#include "bgfx/defines.h"
#include "utils/bgfx.hpp"
#include "velocityFieldCube.hpp"

struct Pos3Vertex
{
    float _x;
    float _y;
    float _z;

    static void init()
    {
        _layout.begin().add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float).end();
    };

    static bgfx::VertexLayout _layout;
};

bgfx::VertexLayout Pos3Vertex::_layout;

Pos3Vertex cubeVertices[] = {{-1.0f, 1.0f, 1.0f},  {1.0f, 1.0f, 1.0f},  {-1.0f, -1.0f, 1.0f},  {1.0f, -1.0f, 1.0f},
                             {-1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, -1.0f}, {-1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, -1.0f}};

struct Pos3TexVertex
{
    float _x;
    float _y;
    float _z;

    float _u;
    float _v;

    static void init()
    {
        _layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .end();
    };

    static bgfx::VertexLayout _layout;
};

bgfx::VertexLayout Pos3TexVertex::_layout;

Pos3TexVertex cubeVerticesTest[] = {
    {-1.0f, 1.0f, 1.0f, 0.0f, 1.0f},   {1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
    {-1.0f, -1.0f, 1.0f, 0.0f, 0.0f},  {1.0f, -1.0f, 1.0f, 1.0f, 0.0f},

    {-1.0f, 1.0f, -1.0f, 0.0f, 1.0f},  {1.0f, 1.0f, -1.0f, 1.0f, 1.0f},
    {-1.0f, -1.0f, -1.0f, 0.0f, 0.0f}, {1.0f, -1.0f, -1.0f, 1.0f, 0.0f},
};

static const uint16_t cubeTriList[] = {2, 1, 0, 2, 3, 1, 5, 6, 4, 7, 6, 5, 4, 2, 0, 6, 2, 4,
                                       3, 5, 1, 3, 7, 5, 1, 4, 0, 1, 5, 4, 6, 3, 2, 7, 3, 6};

VelocityFieldCube::VelocityFieldCube(int simResX, int simResY, int simResZ) : VelocityField(simResX, simResY, simResZ)
{
    // init necessary data for cube rendering
    Pos3Vertex::init();
    Pos3TexVertex::init();
    //_vbhCube = bgfx::createVertexBuffer(bgfx::makeRef(cubeVertices, sizeof(cubeVertices)), Pos3Vertex::_layout);
    _vbhCube =
        bgfx::createVertexBuffer(bgfx::makeRef(cubeVerticesTest, sizeof(cubeVerticesTest)), Pos3TexVertex::_layout);
    _ibhCube = bgfx::createIndexBuffer(bgfx::makeRef(cubeTriList, sizeof(cubeTriList)));
    _vfsCube = shift::loadProgram({"cube_vs.sc", "cube_fs.sc"});
}

VelocityFieldCube::~VelocityFieldCube()
{
    bgfx::destroy(_vbhCube);
    bgfx::destroy(_ibhCube);
}

void VelocityFieldCube::RenderBoundBox()
{
    bgfx::setVertexBuffer(0, _vbhCube);
    bgfx::setIndexBuffer(_ibhCube);
    // bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_PT_LINESTRIP);
    bgfx::setState(BGFX_STATE_DEFAULT);
    bgfx::submit(0, _vfsCube);
}

void VelocityFieldCube::Reset(int _viewID)
{
}

void VelocityFieldCube::AddSource(int _viewID)
{
}

void VelocityFieldCube::Advect(int _viewID)
{
}

void VelocityFieldCube::Project(int _viewID)
{
}
