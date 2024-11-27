#include "bgfx/bgfx.h"
#include "bgfx/defines.h"
#include "fluidSimUtils.hpp"
#include "utils/bgfx.hpp"
#include "utils/common.hpp"
#include "velocityField.hpp"
#include <intrin.h>

VelocityField::VelocityField(int _width, int _height, float _dt, float _speed)
{
    // init uniforms and uniform handle
    _uParams.mousePosX = 3.40e+38; // avoid the origin problem of the addSource compute shader
    _uParams.mousePosY = 3.40e+38;
    _uParams.mouseVelX = 0.0f;
    _uParams.mouseVelY = 0.0f;
    _uParams.radius = 10.0f;
    _uParams.simResX = float(_width);
    _uParams.simResY = float(_height);
    _uParams.simResZ = 1.0f;
    _uParams.persist = float(false);
    _uParams.deltaTime = _dt;
    _uParams.speed = _speed;
    _uParams.offsetX = 0.0f;
    _uParams.offsetY = 0.0f;

    _uhParams = bgfx::createUniform("uParams", bgfx::UniformType::Vec4, int(UniformType::count / 4) + 1);

    // init the compute shader group size
    _groupSizeX = _uParams.simResX / kThreadGroupSizeX;
    _groupSizeY = _uParams.simResY / kThreadGroupSizeY;
    _groupSizeZ = _uParams.simResZ / kThreadGroupSizeZ > 1 ? _uParams.simResZ / kThreadGroupSizeZ : 1;

    // init buffer handles
    bgfx::VertexLayout bufferLayout;
    bufferLayout.begin().add(bgfx::Attrib::TexCoord0, 1, bgfx::AttribType::Float).end();

    _prevVelX = bgfx::createDynamicVertexBuffer(_uParams.simResX * _uParams.simResY * _uParams.simResZ, bufferLayout,
                                                BGFX_BUFFER_COMPUTE_READ_WRITE);
    _prevVelY = bgfx::createDynamicVertexBuffer(_uParams.simResX * _uParams.simResY * _uParams.simResZ, bufferLayout,
                                                BGFX_BUFFER_COMPUTE_READ_WRITE);
    _curVelX = bgfx::createDynamicVertexBuffer(_uParams.simResX * _uParams.simResY * _uParams.simResZ, bufferLayout,
                                               BGFX_BUFFER_COMPUTE_READ_WRITE);
    _curVelY = bgfx::createDynamicVertexBuffer(_uParams.simResX * _uParams.simResY * _uParams.simResZ, bufferLayout,
                                               BGFX_BUFFER_COMPUTE_READ_WRITE);
    _isFluid = bgfx::createDynamicVertexBuffer(_uParams.simResX * _uParams.simResY * _uParams.simResZ, bufferLayout,
                                               BGFX_BUFFER_COMPUTE_READ_WRITE);
    _divergence = bgfx::createDynamicVertexBuffer(_uParams.simResX * _uParams.simResY * _uParams.simResZ, bufferLayout,
                                                  BGFX_BUFFER_COMPUTE_READ_WRITE);

    // init compute shaders
    _csReset = shift::loadProgram({"velocity_reset_cs.sc"});
    _csAddSource = shift::loadProgram({"velocity_addSource_cs.sc"});
    _csAdvect = shift::loadProgram({"velocity_advect_cs.sc"});
    _csProject = shift::loadProgram({"velocity_project_cs.sc"});
    _dispDivergence = shift::loadProgram({"quad_vs.sc", "quad_divergence_fs.sc"});
    _dispAdvect = shift::loadProgram({"quad_vs.sc", "quad_advect_fs.sc"});
    _dispProject = shift::loadProgram({"quad_vs.sc", "quad_project_fs.sc"});
}

VelocityField::~VelocityField()
{
    bgfx::destroy(_uhParams);
    bgfx::destroy(_prevVelX);
    bgfx::destroy(_prevVelY);
    bgfx::destroy(_curVelX);
    bgfx::destroy(_curVelY);
    bgfx::destroy(_isFluid);
    bgfx::destroy(_csReset);
    bgfx::destroy(_csAddSource);
    bgfx::destroy(_csAdvect);
    bgfx::destroy(_csProject);
}

void VelocityField::Reset(int _viewID)
{
    bgfx::setBuffer(0, _prevVelX, bgfx::Access::Write);
    bgfx::setBuffer(1, _prevVelY, bgfx::Access::Write);
    bgfx::setBuffer(2, _curVelX, bgfx::Access::Write);
    bgfx::setBuffer(3, _curVelY, bgfx::Access::Write);
    bgfx::setBuffer(4, _isFluid, bgfx::Access::Write);
    bgfx::setUniform(_uhParams, &_uParams, int(UniformType::count / 4) + 1);
    bgfx::dispatch(_viewID, _csReset, _groupSizeX, _groupSizeY, _groupSizeZ);
}

void VelocityField::AddSource(int _viewID)
{
    bgfx::setBuffer(0, _prevVelX, bgfx::Access::Read);
    bgfx::setBuffer(1, _prevVelY, bgfx::Access::Read);
    bgfx::setBuffer(2, _curVelX, bgfx::Access::ReadWrite);
    bgfx::setBuffer(3, _curVelY, bgfx::Access::ReadWrite);
    // might need to change the access????
    bgfx::setBuffer(4, _isFluid, bgfx::Access::Write);
    bgfx::setUniform(_uhParams, &_uParams, int(UniformType::count / 4) + 1);
    bgfx::dispatch(_viewID, _csAddSource, _groupSizeX, _groupSizeY, _groupSizeZ);
}

void VelocityField::Advect(int _viewID)
{
    bgfx::setBuffer(0, _prevVelX, bgfx::Access::Read);
    bgfx::setBuffer(1, _prevVelY, bgfx::Access::Read);
    bgfx::setBuffer(2, _curVelX, bgfx::Access::ReadWrite);
    bgfx::setBuffer(3, _curVelY, bgfx::Access::ReadWrite);
    bgfx::setBuffer(4, _isFluid, bgfx::Access::Read);
    bgfx::setUniform(_uhParams, &_uParams, int(UniformType::count / 4) + 1);
    bgfx::dispatch(_viewID, _csAdvect, _groupSizeX, _groupSizeY, _groupSizeZ);
}

// Gauss-Seidel method to solve the linear equation
// check https://en.wikipedia.org/wiki/Gauss%E2%80%93Seidel_method
void VelocityField::Project(int _viewID)
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
                updateUniforms(UniformType::offsetX, float(passX));
                updateUniforms(UniformType::offsetY, float(passY));

                bgfx::setBuffer(0, _curVelX, bgfx::Access::ReadWrite);
                bgfx::setBuffer(1, _curVelY, bgfx::Access::ReadWrite);
                bgfx::setBuffer(2, _isFluid, bgfx::Access::Read);
                bgfx::setBuffer(3, _divergence, bgfx::Access::Read);
                bgfx::setUniform(_uhParams, &_uParams, int(UniformType::count / 4) + 1);
                bgfx::dispatch(_viewID, _csProject, _groupSizeX, _groupSizeY, _groupSizeZ);
            }
        }
    }
}

void VelocityField::DispDiv(int _viewID)
{
    /* the vertex buffer and index buffer are set outside */

    // set compute buffer
    bgfx::setBuffer(0, _divergence, bgfx::Access::Read);
    bgfx::setBuffer(1, _isFluid, bgfx::Access::Read);

    // check https://bkaradzic.github.io/bgfx/bgfx.html#_CPPv4N4bgfx7Encoder8setStateE8uint64_t8uint32_t
    bgfx::setState(BGFX_STATE_DEFAULT);
    bgfx::submit(_viewID, _dispDivergence);
}

void VelocityField::DispAdvect(int _viewID)
{
    // set compute buffer
    bgfx::setBuffer(0, _isFluid, bgfx::Access::Read);
    bgfx::setBuffer(1, _curVelX, bgfx::Access::Read);
    bgfx::setBuffer(2, _curVelY, bgfx::Access::Read);

    // check https://bkaradzic.github.io/bgfx/bgfx.html#_CPPv4N4bgfx7Encoder8setStateE8uint64_t8uint32_t
    bgfx::setState(BGFX_STATE_DEFAULT);
    bgfx::submit(_viewID, _dispAdvect);
}

void VelocityField::DispProject(int _viewID)
{
    // set compute buffer
    bgfx::setBuffer(0, _isFluid, bgfx::Access::Read);
    bgfx::setBuffer(1, _curVelX, bgfx::Access::Read);
    bgfx::setBuffer(2, _curVelY, bgfx::Access::Read);

    // check https://bkaradzic.github.io/bgfx/bgfx.html#_CPPv4N4bgfx7Encoder8setStateE8uint64_t8uint32_t
    bgfx::setState(BGFX_STATE_DEFAULT);
    bgfx::submit(_viewID, _dispProject);
}
