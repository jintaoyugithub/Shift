#pragma once

#include "bgfx/bgfx.h"
#include "velocityField.hpp"
#include <utils/common.hpp>

class VelocityFieldCube final : public VelocityField
{
  public:
    int solverItr = 50;

  public:
    VelocityFieldCube(int simResX, int simResY, int simResZ);
    ~VelocityFieldCube();

    // Debug rendering
    void RenderBoundBox();

  private:
    void Reset(int _viewID) override;
    void AddSource(int _viewID) override;
    void Advect(int _viewID) override;
    void Project(int _viewID) override;

  private:
    // for cube rendering
    bgfx::VertexBufferHandle _vbhCube;
    bgfx::IndexBufferHandle _ibhCube;
    // vfs means vertex and fragment shaders
    bgfx::ProgramHandle _vfsCube;
};
