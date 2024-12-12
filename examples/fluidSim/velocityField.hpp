#pragma once

#include <utils/common.hpp>

enum ProgramType
{
    Reset,
    AddSource,
    Advect,
    Project,
    RenderBoundary,
};

enum BufferType
{
    PrevVelX,
    PrevVelY,
    PrevVelZ,
    CurVelX,
    CurVelY,
    CurVelZ,
    IsFluid,
    Divergence,
};

struct uParams
{
    float interPosX;
    float interPosY;
    float interPosZ;

    float interVelX;
    float interVelY;
    float interVelZ;

    float radius;

    float simResX;
    float simResY;
    float simResZ;

    float deltaTime;
    float speed;

    // these offsets are used for staggering the clearing of the divergence for the corresponding cell
    float divOffsetX;
    float divOffsetY;
    float divOffsetZ;
};

enum UniformType
{
    InterPosX,
    InterPosY,
    InterPosZ,

    InterVelX,
    InterVelY,
    InterVelZ,

    Radius,

    SimResX,
    SimResY,
    SimResZ,

    DeltaTime,
    Speed,

    DivOffsetX,
    DivOffsetY,
    DivOffsetZ,

    Count,
};

class VelocityField
{
  protected:
    virtual void Reset(int _viewID) {};
    virtual void AddSource(int _viewID) {};
    virtual void Advect(int _viewID) {};
    virtual void Project(int _viewID) {};
    virtual void RenderBoundary(int _viewID) {};

  public:
    VelocityField(int simResX, int simResY, int simResZ);
    virtual ~VelocityField();

  public:
    inline void dispatch(ProgramType _type, int _viewID)
    {
        switch (_type)
        {
        case ProgramType::Reset:
            Reset(_viewID);
            break;
        case ProgramType::AddSource:
            AddSource(_viewID);
            break;
        case ProgramType::Advect:
            Advect(_viewID);
            break;
        case ProgramType::Project:
            Project(_viewID);
            break;
        case ProgramType::RenderBoundary:
            RenderBoundary(_viewID);
            break;
        }
    }

    inline bgfx::DynamicVertexBufferHandle &getBufferHandle(BufferType _type)
    {
        switch (_type)
        {
        case BufferType::PrevVelX:
            return _prevVelX;
            break;
        case BufferType::PrevVelY:
            return _prevVelY;
            break;
        case BufferType::PrevVelZ:
            return _prevVelZ;
            break;
        case BufferType::CurVelX:
            return _curVelX;
            break;
        case BufferType::CurVelY:
            return _curVelY;
            break;
        case BufferType::CurVelZ:
            return _curVelZ;
            break;
        case BufferType::IsFluid:
            return _isFluid;
            break;
        case BufferType::Divergence:
            return _divergence;
            break;
        }
    }

    inline bgfx::ProgramHandle getProgramHandle(ProgramType _type)
    {
        switch (_type)
        {
        case ProgramType::Reset:
            return _csReset;
            break;
        case ProgramType::AddSource:
            return _csAddSource;
            break;
        case ProgramType::Advect:
            return _csAdvect;
            break;
        case ProgramType::Project:
            return _csProject;
            break;
        }
        return BGFX_INVALID_HANDLE;
    }

    inline void updateUniforms(UniformType _type, float val)
    {
        switch (_type)
        {
        case UniformType::InterPosX:
            _uParams.interPosX = val;
            break;
        case UniformType::InterPosY:
            _uParams.interPosY = val;
            break;
        case UniformType::InterPosZ:
            _uParams.interPosZ = val;
            break;
        case UniformType::InterVelX:
            _uParams.interVelX = val;
            break;
        case UniformType::InterVelY:
            _uParams.interVelY = val;
            break;
        case UniformType::InterVelZ:
            _uParams.interVelZ = val;
            break;
        case UniformType::Radius:
            _uParams.radius = val;
            break;
        case UniformType::SimResX:
            _uParams.simResX = val;
            break;
        case UniformType::SimResY:
            _uParams.simResY = val;
            break;
        case UniformType::SimResZ:
            _uParams.simResZ = val;
            break;
        case UniformType::DeltaTime:
            _uParams.deltaTime = val;
            break;
        case UniformType::Speed:
            _uParams.speed = val;
            break;
        case UniformType::DivOffsetX:
            _uParams.divOffsetX = val;
            break;
        case UniformType::DivOffsetY:
            _uParams.divOffsetY = val;
            break;
        case UniformType::DivOffsetZ:
            _uParams.divOffsetZ = val;
            break;
        }
    }

    inline float getUniforms(UniformType _type)
    {
        switch (_type)
        {
        case UniformType::SimResX:
            return _uParams.simResX;
        case UniformType::SimResY:
            return _uParams.simResY;
        case UniformType::SimResZ:
            return _uParams.simResZ;
        }

        return 0.0f;
    }

  public:
    int solverItr = 50;

  protected:
    // group size of compute shader
    int _groupSizeX;
    int _groupSizeY;
    int _groupSizeZ;

    uParams _uParams;
    bgfx::UniformHandle _uhParams;

    // buffers used in compute shader
    bgfx::DynamicVertexBufferHandle _prevVelX;
    bgfx::DynamicVertexBufferHandle _prevVelY;
    bgfx::DynamicVertexBufferHandle _prevVelZ;

    bgfx::DynamicVertexBufferHandle _curVelX;
    bgfx::DynamicVertexBufferHandle _curVelY;
    bgfx::DynamicVertexBufferHandle _curVelZ;

    // this buffer is to set boundary or obstacles in the fluid sim area
    bgfx::DynamicVertexBufferHandle _isFluid;
    bgfx::DynamicVertexBufferHandle _divergence;

    bgfx::ProgramHandle _csReset;
    bgfx::ProgramHandle _csAddSource;
    bgfx::ProgramHandle _csAdvect;
    bgfx::ProgramHandle _csProject;
};
