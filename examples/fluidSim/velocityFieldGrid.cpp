#include "bgfx/bgfx.h"
#include "bgfx/defines.h"
#include "fluidSimUtils.hpp"
#include "utils/bgfx.hpp"
#include "utils/common.hpp"
#include "velocityFieldGrid.hpp"
#include <intrin.h>

VelocityFieldGrid::VelocityFieldGrid(int simResX, int simResY, int simResZ) : VelocityField(simResX, simResY, simResZ)
{
    // init compute shaders
    _csReset = shift::loadProgram({"velocity_reset_cs.sc"});
    _csAddSource = shift::loadProgram({"velocity_addSource_cs.sc"});
    _csAdvect = shift::loadProgram({"velocity_advect_cs.sc"});
    _csProject = shift::loadProgram({"velocity_project_cs.sc"});
    _dispDivergence = shift::loadProgram({"quad_vs.sc", "quad_divergence_fs.sc"});
    _dispAdvect = shift::loadProgram({"quad_vs.sc", "quad_advect_fs.sc"});
    _dispProject = shift::loadProgram({"quad_vs.sc", "quad_project_fs.sc"});
}

VelocityFieldGrid::~VelocityFieldGrid()
{
    bgfx::destroy(_dispDivergence);
    bgfx::destroy(_dispAdvect);
    bgfx::destroy(_dispProject);
}

void VelocityFieldGrid::Reset(int _viewID)
{
    bgfx::setBuffer(0, _prevVelX, bgfx::Access::Write);
    bgfx::setBuffer(1, _prevVelY, bgfx::Access::Write);
    bgfx::setBuffer(2, _curVelX, bgfx::Access::Write);
    bgfx::setBuffer(3, _curVelY, bgfx::Access::Write);
    bgfx::setBuffer(4, _isFluid, bgfx::Access::Write);
    bgfx::setUniform(_uhParams, &_uParams, int(UniformType::Count / 4) + 1);
    bgfx::dispatch(_viewID, _csReset, _groupSizeX, _groupSizeY, _groupSizeZ);
}

void VelocityFieldGrid::AddSource(int _viewID)
{
    bgfx::setBuffer(0, _prevVelX, bgfx::Access::Read);
    bgfx::setBuffer(1, _prevVelY, bgfx::Access::Read);
    bgfx::setBuffer(2, _curVelX, bgfx::Access::ReadWrite);
    bgfx::setBuffer(3, _curVelY, bgfx::Access::ReadWrite);
    // might need to change the access????
    bgfx::setBuffer(4, _isFluid, bgfx::Access::Write);
    bgfx::setUniform(_uhParams, &_uParams, int(UniformType::Count / 4) + 1);
    bgfx::dispatch(_viewID, _csAddSource, _groupSizeX, _groupSizeY, _groupSizeZ);
}

void VelocityFieldGrid::Advect(int _viewID)
{
    bgfx::setBuffer(0, _prevVelX, bgfx::Access::Read);
    bgfx::setBuffer(1, _prevVelY, bgfx::Access::Read);
    bgfx::setBuffer(2, _curVelX, bgfx::Access::ReadWrite);
    bgfx::setBuffer(3, _curVelY, bgfx::Access::ReadWrite);
    bgfx::setBuffer(4, _isFluid, bgfx::Access::Read);
    bgfx::setUniform(_uhParams, &_uParams, int(UniformType::Count / 4) + 1);
    bgfx::dispatch(_viewID, _csAdvect, _groupSizeX, _groupSizeY, _groupSizeZ);
}

// Gauss-Seidel method to solve the linear equation
// check https://en.wikipedia.org/wiki/Gauss%E2%80%93Seidel_method
void VelocityFieldGrid::Project(int _viewID)
{
    for (int i = 0; i < solverItr; i++)
    {
        for (int passX = 0; passX < 2; passX++)
        {
            for (int passY = 0; passY < 2; passY++)
            {
                /*
                   The first offset (0,0), assume we have a 5x5 grid
                   |1|0|1|0|1|
                   |0|1|0|1|0|
                   |1|0|1|0|1|
                   |0|1|0|1|0|
                   |1|0|1|0|1|
                   1 means need to be cleaned

                   Next offset (1,0) will be:
                   |0|1|0|1|0|
                   |1|0|1|0|1|
                   |0|1|0|1|0|
                   |1|0|1|0|1|
                   |0|1|0|1|0|
                 */
                updateUniforms(UniformType::DivOffsetX, float(passX));
                updateUniforms(UniformType::DivOffsetY, float(passY));

                bgfx::setBuffer(0, _curVelX, bgfx::Access::ReadWrite);
                bgfx::setBuffer(1, _curVelY, bgfx::Access::ReadWrite);
                bgfx::setBuffer(2, _isFluid, bgfx::Access::Read);
                bgfx::setBuffer(3, _divergence, bgfx::Access::Write);
                bgfx::setUniform(_uhParams, &_uParams, int(UniformType::Count / 4) + 1);
                bgfx::dispatch(_viewID, _csProject, _groupSizeX, _groupSizeY, _groupSizeZ);
            }
        }
    }
}

void VelocityFieldGrid::DispDiv(int _viewID)
{
    /* the vertex buffer and index buffer are set outside */

    // set compute buffer
    bgfx::setBuffer(0, _divergence, bgfx::Access::Read);
    bgfx::setBuffer(1, _isFluid, bgfx::Access::Read);

    // check https://bkaradzic.github.io/bgfx/bgfx.html#_CPPv4N4bgfx7Encoder8setStateE8uint64_t8uint32_t
    bgfx::setState(BGFX_STATE_DEFAULT);
    bgfx::submit(_viewID, _dispDivergence);
}

void VelocityFieldGrid::DispAdvect(int _viewID)
{
    // set compute buffer
    bgfx::setBuffer(0, _isFluid, bgfx::Access::Read);
    bgfx::setBuffer(1, _curVelX, bgfx::Access::Read);
    bgfx::setBuffer(2, _curVelY, bgfx::Access::Read);

    // check https://bkaradzic.github.io/bgfx/bgfx.html#_CPPv4N4bgfx7Encoder8setStateE8uint64_t8uint32_t
    bgfx::setState(BGFX_STATE_DEFAULT);
    bgfx::submit(_viewID, _dispAdvect);
}

void VelocityFieldGrid::DispProject(int _viewID)
{
    // set compute buffer
    bgfx::setBuffer(0, _isFluid, bgfx::Access::Read);
    bgfx::setBuffer(1, _curVelX, bgfx::Access::Read);
    bgfx::setBuffer(2, _curVelY, bgfx::Access::Read);

    // check https://bkaradzic.github.io/bgfx/bgfx.html#_CPPv4N4bgfx7Encoder8setStateE8uint64_t8uint32_t
    bgfx::setState(BGFX_STATE_DEFAULT);
    bgfx::submit(_viewID, _dispProject);
}
