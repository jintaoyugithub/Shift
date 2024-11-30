#pragma once

#include "bgfx/bgfx.h"
#include <utils/common.hpp>

struct Pos3Vertex {
  float _x;
  float _y;
  float _z;

  static void init() {
    _layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .end();
  };

  static bgfx::VertexLayout _layout;
};

bgfx::VertexLayout Pos3Vertex::_layout;

Pos3Vertex cubeVertices[] = {{-1.0f, 1.0f, 1.0f},   {1.0f, 1.0f, 1.0f},
                             {-1.0f, -1.0f, 1.0f},  {1.0f, -1.0f, 1.0f},
                             {-1.0f, 1.0f, -1.0f},  {1.0f, 1.0f, -1.0f},
                             {-1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, -1.0f}};

static const uint16_t cubeTriList[] = {2, 1, 0, 2, 3, 1, 5, 6, 4, 7, 6, 5,
                                       4, 2, 0, 6, 2, 4, 3, 5, 1, 3, 7, 5,
                                       1, 4, 0, 1, 5, 4, 6, 3, 2, 7, 3, 6};

struct uParams {
  // input position
  float objPosX;
  float objPosY;
  float objPosZ;

  // input weight influence
  float radius;

  // sim resolution
  float simResX;
  float simResY;
  float simResZ;
};

class VelocityCube {
public:
  int solverItr = 50;

public:
  VelocityCube(int simResX, int simResY, int simResZ, float dt, float speed);
  ~VelocityCube();

private:
  void AddSource(int _viewID);

private:
  // for cube rendering
  bgfx::VertexBufferHandle _vbhCube;
  bgfx::IndexBufferHandle _ibhCube;

  uParams _uParams;
  bgfx::UniformHandle _uhParams;

  // buffers for compute shader
  bgfx::DynamicVertexBufferHandle _isFluid;

  bgfx::ProgramHandle _cubeRendering;
  bgfx::ProgramHandle _csReset;
  bgfx::ProgramHandle _csAddSource;
};
