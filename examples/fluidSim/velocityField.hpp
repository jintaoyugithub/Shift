#pragma once

#include "bgfx/bgfx.h"
#include "spirv-headers/include/spirv/unified1/spirv.hpp11"
#include <utils/common.hpp>

enum ProgramType
{
    reset,
    addSource,
    advect,
    project
};

enum BufferType
{
    prevVelX,
    prevVelY,
    curVelX,
    curVelY,
    isFluid,
};

struct uParams
{
    float mousePosX;
    float mousePosY;
    float mouseVelX;
    float mouseVelY;

    float radius;

    float simResX;
    float simResY;
    float simResZ;

    float persist;

    float deltaTime;
    float speed;
};

enum UniformType
{
    mousePosX,
    mousePosY,
    mouseVelX,
    mouseVelY,

    radius,

    simResX,
    simResY,
    simResZ,

    persist,

    deltaTime,
    speed,

    count,
};

class VelocityField
{
  private:
    void Reset(int _viewID);
    void AddSource(int _viewID);
    void Advect(int _viewID);
    void Project(int _viewID);

  public:
    // VelocityField(int width, int height, float dt, float speed);
    VelocityField();
    ~VelocityField();

  public:
    inline void dispatch(ProgramType _type, int _viewID)
    {
        switch (_type)
        {
        case ProgramType::reset:
            Reset(_viewID);
            break;
        case ProgramType::addSource:
            AddSource(_viewID);
            break;
        case ProgramType::advect:
            Advect(_viewID);
            break;
        case ProgramType::project:
            Project(_viewID);
            break;
        }
    }

    inline bgfx::DynamicVertexBufferHandle getBufferHandle(BufferType _type)
    {
        switch (_type)
        {
        case BufferType::prevVelX:
            return _prevVelX;
            break;
        case BufferType::prevVelY:
            return _prevVelY;
            break;
        case BufferType::curVelX:
            return _curVelX;
            break;
        case BufferType::curVelY:
            return _curVelY;
        case BufferType::isFluid:
            return _isFluid;
            break;
        }
        return BGFX_INVALID_HANDLE;
    }

    inline bgfx::ProgramHandle getProgramHandle(ProgramType _type)
    {
        switch (_type)
        {
        case ProgramType::reset:
            return _csReset;
        case ProgramType::advect:
            return _csAdvect;
        case ProgramType::project:
            return _csProject;
        }
        return BGFX_INVALID_HANDLE;
    }

    inline void updateUniforms(UniformType _type, float val)
    {
        switch (_type)
        {
        case UniformType::mousePosX:
            _uParams.mousePosX = val;
            break;
        case UniformType::mousePosY:
            _uParams.mousePosY = val;
            break;
        case UniformType::mouseVelX:
            _uParams.mouseVelX = val;
            break;
        case UniformType::mouseVelY:
            _uParams.mouseVelY = val;
            break;
        case UniformType::radius:
            _uParams.radius = val;
            break;
        case UniformType::simResX:
            _uParams.simResX = val;
            break;
        case UniformType::simResY:
            _uParams.simResY = val;
            break;
        case UniformType::simResZ:
            _uParams.simResZ = val;
            break;
        case UniformType::persist:
            _uParams.persist = val;
            break;
        case UniformType::deltaTime:
            _uParams.deltaTime = val;
            break;
        case UniformType::speed:
            _uParams.speed = val;
            break;
        }
    }

  private:
    int _groupSizeX;
    int _groupSizeY;
    int _groupSizeZ;

    uParams _uParams;
    bgfx::UniformHandle _uhParams;

    // buffers used in compute shader
    bgfx::DynamicVertexBufferHandle _prevVelX;
    bgfx::DynamicVertexBufferHandle _prevVelY;
    bgfx::DynamicVertexBufferHandle _curVelX;
    bgfx::DynamicVertexBufferHandle _curVelY;
    // this buffer is to set boundary or obstacles in the fluid sim area
    bgfx::DynamicVertexBufferHandle _isFluid;

    bgfx::ProgramHandle _csReset;
    bgfx::ProgramHandle _csAddSource;
    bgfx::ProgramHandle _csAdvect;
    bgfx::ProgramHandle _csProject;
};
