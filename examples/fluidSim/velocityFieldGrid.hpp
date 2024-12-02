#pragma once

#include "bgfx/bgfx.h"
#include "spirv-headers/include/spirv/unified1/spirv.hpp11"
#include "velocityField.hpp"
#include <utils/common.hpp>

class VelocityFieldGrid final : public VelocityField
{
  private:
    void Reset(int _viewID) override;
    void AddSource(int _viewID) override;
    void Advect(int _viewID) override;
    void Project(int _viewID) override;

  public:
    VelocityFieldGrid(int simResX, int simResY, int simResZ);
    // VelocityField();
    ~VelocityFieldGrid();

    // debug
    void DispDiv(int _viewID);
    void DispAdvect(int _viewID);
    void DispProject(int _viewID);

  private:
    // for debug
    bgfx::ProgramHandle _dispDivergence;
    bgfx::ProgramHandle _dispAdvect;
    bgfx::ProgramHandle _dispProject;
};
