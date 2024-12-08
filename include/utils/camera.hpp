#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace shift
{
// TODO input class
enum Property
{
    ViewMat,
    InvViewMat,
    ProjMat,
    InvProjMat,
    Position,
    ViewDir,
    Fov,
    NearPlane,
    FarPlane,
    MoveSpeed,
    RotateSpeed,
};

class Camera
{
  public:
    Camera(GLFWwindow *window, uint32_t viewPortWidth, uint32_t viewPortHeight);

    // maybe virtual in the furture
    // because it's might be the base class
    ~Camera() = default;

    void OnUpdate(float ts);
    void OnResize(uint32_t width, uint32_t height);

    /* Get all matrices */
    template <typename T> const T &GetProperties(Property _property) const;

    template <> inline const glm::vec3 &GetProperties(Property _property) const
    {
        switch (_property)
        {
        case Property::Position:
            return _pos;
            break;
        case Property::ViewDir:
            return _viewDir;
            break;
        }
    }

    template <> inline const glm::mat4 &GetProperties(Property _property) const
    {
        switch (_property)
        {
        case Property::ViewMat:
            return _viewMat;
            break;
        case Property::ProjMat:
            return _projMat;
            break;
        }
    }

    /* Update camera properties */
    template <typename T> inline void UpdateProperties(Property _property, T value)
    {
        switch (_property)
        {
        case Property::Fov:
            _fov = value;
            break;
        case Property::NearPlane:
            _near = value;
            break;
        case Property::FarPlane:
            _far = value;
            break;
        case Property::MoveSpeed:
            _moveSpeed = value;
            break;
        case Property::RotateSpeed:
            _rotateSpped = value;
            break;
        }
    }

  private:
    void RecalViewMat();
    void RecalProjMat();

  private:
    /* Camera properties */
    glm::vec3 _pos{glm::vec3(0.0, 0.0, 5.0)};
    // camera coord system
    // The camera coord system of OpenGL is right hand system
    // But the positive view dir is negative Z
    glm::vec3 _upDir;
    glm::vec3 _rightDir;
    glm::vec3 _viewDir{glm::vec3(0.0, 0.0, -1.0f)};

    float _fov{45.0f};
    float _near{0.1f};
    float _far{100.0f};
    float _moveSpeed{5.0f};
    float _rotateSpped{.3f};

    // shoud not be here, should be intergrated into the input class
    glm::vec2 _lastMousePos;
    GLFWwindow *_window;
    // should have a window manager, because appbaseglfw also need the info
    // from the window, for example, window width, height, viewport width, height and so on
    uint32_t _viewPortWidht = 0;
    uint32_t _viewPortHeight = 0;

    glm::mat4 _viewMat{1.0f};
    glm::mat4 _projMat{1.0f};
    glm::mat4 _invViewMat{1.0f};
    glm::mat4 _invProjMat{1.0f};
};
} // namespace shift
