#include "bgfx/bgfx.h"
#include "bgfx/defines.h"
#include "fluidSimUtils.hpp"
#include "utils/bgfx.hpp"
#include "velocityField.hpp"
#include <intrin.h>

VelocityField::VelocityField(int width, int height, float dt, float speed)
{
    // init the compute shader group size
    _groupSizeX = _uParams.simResX / kThreadGroupSizeX;
    _groupSizeY = _uParams.simResY / kThreadGroupSizeY;
    _groupSizeZ = _uParams.simResZ / kThreadGroupSizeZ > 1 ? _uParams.simResZ / kThreadGroupSizeZ : 1;

    // init uniforms and uniform handle
    _uParams.mousePosX = 3.40e+38; // avoid the origin problem of the addSource compute shader
    _uParams.mousePosY = 3.40e+38;
    _uParams.mouseVelX = 0.0f;
    _uParams.mouseVelY = 0.0f;
    _uParams.radius = 5.0f;
    _uParams.simResX = width;
    _uParams.simResY = height;
    _uParams.simResZ = 1;
    _uParams.persist = false;
    _uParams.deltaTime = dt;
    _uParams.speed = speed;

    _uhParams = bgfx::createUniform("uParams", bgfx::UniformType::Vec4, int(UniformType::count / 4));

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

    // init compute shaders
    _csReset = shift::loadProgram({"velocity_init_cs.sc"});
    _csAddSource = shift::loadProgram({"velocity_addSource_cs.sc"});
    _csAdvect = shift::loadProgram({"velocity_advect_cs.sc"});
    _csProject = shift::loadProgram({"velocity_project_cs.sc"});
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
    bgfx::setUniform(_uhParams, &_uParams);
    bgfx::dispatch(_viewID, _csReset, _groupSizeX, _groupSizeY, _groupSizeZ);
}

void VelocityField::AddSource(int _viewID)
{
    bgfx::setBuffer(0, _prevVelX, bgfx::Access::Read);
    bgfx::setBuffer(1, _prevVelY, bgfx::Access::Read);
    bgfx::setBuffer(2, _curVelX, bgfx::Access::ReadWrite);
    bgfx::setBuffer(3, _curVelY, bgfx::Access::ReadWrite);
    // might need to change the access????
    bgfx::setBuffer(4, _isFluid, bgfx::Access::Read);
    bgfx::setUniform(_uhParams, &_uParams);
    bgfx::dispatch(_viewID, _csAddSource, _groupSizeX, _groupSizeY, _groupSizeZ);
}

void VelocityField::Advect(int _viewID)
{
    bgfx::setBuffer(0, _prevVelX, bgfx::Access::Read);
    bgfx::setBuffer(1, _prevVelY, bgfx::Access::Read);
    bgfx::setBuffer(2, _curVelX, bgfx::Access::ReadWrite);
    bgfx::setBuffer(3, _curVelY, bgfx::Access::ReadWrite);
    bgfx::setBuffer(4, _isFluid, bgfx::Access::Read);
    bgfx::setUniform(_uhParams, &_uParams);
    bgfx::dispatch(_viewID, _csAdvect, _groupSizeX, _groupSizeY, _groupSizeZ);
}

void VelocityField::Project(int _viewID)
{
    bgfx::setBuffer(0, _prevVelX, bgfx::Access::Read);
    bgfx::setBuffer(1, _prevVelY, bgfx::Access::Read);
    bgfx::setBuffer(2, _curVelX, bgfx::Access::ReadWrite);
    bgfx::setBuffer(3, _curVelY, bgfx::Access::ReadWrite);
    bgfx::setBuffer(4, _isFluid, bgfx::Access::Read);
    bgfx::setUniform(_uhParams, &_uParams);
    bgfx::dispatch(_viewID, _csProject, _groupSizeX, _groupSizeY, _groupSizeZ);
}
