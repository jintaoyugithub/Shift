#include "fluidSimUtils.hpp"
#include "velocityField.hpp"
#include <utils/common.hpp>

VelocityField::VelocityField(int simResX, int simResY, int simResZ)
{
    // init uniforms and uniform handle
    _uParams.interPosX = 3.40e+38; // avoid the origin problem of the addSource compute shader
    _uParams.interPosY = 3.40e+38;
    _uParams.interPosZ = 3.40e+38;
    _uParams.interVelX = 0.0f;
    _uParams.interVelY = 0.0f;
    _uParams.interVelZ = 0.0f;
    _uParams.radius = 20.0f;
    _uParams.simResX = float(simResX);
    _uParams.simResY = float(simResY);
    _uParams.simResZ = float(simResZ);
    _uParams.deltaTime = 0.02f;
    _uParams.speed = 75.0f;
    _uParams.divOffsetX = 0.0f;
    _uParams.divOffsetY = 0.0f;
    _uParams.divOffsetZ = 0.0f;

    // +1 is because when
    _uhParams = bgfx::createUniform("uParams", bgfx::UniformType::Vec4, int(UniformType::Count / 4) + 1);

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
    _prevVelZ = bgfx::createDynamicVertexBuffer(_uParams.simResX * _uParams.simResY * _uParams.simResZ, bufferLayout,
                                                BGFX_BUFFER_COMPUTE_READ_WRITE);
    _curVelX = bgfx::createDynamicVertexBuffer(_uParams.simResX * _uParams.simResY * _uParams.simResZ, bufferLayout,
                                               BGFX_BUFFER_COMPUTE_READ_WRITE);
    _curVelY = bgfx::createDynamicVertexBuffer(_uParams.simResX * _uParams.simResY * _uParams.simResZ, bufferLayout,
                                               BGFX_BUFFER_COMPUTE_READ_WRITE);
    _curVelZ = bgfx::createDynamicVertexBuffer(_uParams.simResX * _uParams.simResY * _uParams.simResZ, bufferLayout,
                                               BGFX_BUFFER_COMPUTE_READ_WRITE);
    _isFluid = bgfx::createDynamicVertexBuffer(_uParams.simResX * _uParams.simResY * _uParams.simResZ, bufferLayout,
                                               BGFX_BUFFER_COMPUTE_READ_WRITE);
    _divergence = bgfx::createDynamicVertexBuffer(_uParams.simResX * _uParams.simResY * _uParams.simResZ, bufferLayout,
                                                  BGFX_BUFFER_COMPUTE_READ_WRITE);

    // program handles init should be in child class
}

VelocityField::~VelocityField()
{
    // destory uniform handles
    bgfx::destroy(_uhParams);

    // destory buffers
    bgfx::destroy(_prevVelX);
    bgfx::destroy(_prevVelY);
    bgfx::destroy(_prevVelZ);
    bgfx::destroy(_curVelX);
    bgfx::destroy(_curVelY);
    bgfx::destroy(_curVelZ);
    bgfx::destroy(_isFluid);
    bgfx::destroy(_divergence);

    // destory program handles
    bgfx::destroy(_csReset);
    bgfx::destroy(_csAddSource);
    bgfx::destroy(_csAdvect);
    bgfx::destroy(_csProject);
}
