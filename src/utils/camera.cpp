#include <GLFW/glfw3.h>
#include <utils/camera.hpp>

namespace shift
{
// input system
bool isKeyDown(GLFWwindow *_window, int key)
{
    int state = glfwGetKey(_window, key);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

Camera::Camera(GLFWwindow *window, uint32_t viewPortWidth, uint32_t viewPortHeight)
    : _window(window), _viewPortWidht(viewPortWidth), _viewPortHeight(viewPortHeight)
{
    glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
    _rightDir = glm::cross(_viewDir, up);
    _upDir = glm::cross(_rightDir, _viewDir);

    // init the view and proj matrices
    RecalViewMat();
    RecalProjMat();
}

void Camera::OnUpdate(float ts)
{
    // get mouse pos
    double x, y;
    glfwGetCursorPos(_window, &x, &y);
    glm::vec2 mousePos = glm::vec2(x, y);
    // get mouse delta pos
    glm::vec2 deltaMousePos = (mousePos - _lastMousePos) * ts;
    // update last mouse pos
    _lastMousePos = mousePos;

    // Every actions happen here need the right mouse button to be pressed
    // Should be intergrated into input system
    if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS)
    {
        // glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        return;
    }

    bool isMoved = false;

    // camera movements
    if (isKeyDown(_window, GLFW_KEY_W))
    {
        _pos += _viewDir * _moveSpeed * ts;
        isMoved = true;
    }

    if (isKeyDown(_window, GLFW_KEY_S))
    {
        _pos -= _viewDir * _moveSpeed * ts;
        isMoved = true;
    }

    if (isKeyDown(_window, GLFW_KEY_A))
    {
        _pos -= _rightDir * _moveSpeed * ts;
        isMoved = true;
    }

    if (isKeyDown(_window, GLFW_KEY_D))
    {
        _pos += _rightDir * _moveSpeed * ts;
        isMoved = true;
    }

    if (isKeyDown(_window, GLFW_KEY_Q))
    {
        _pos += _upDir * _moveSpeed * ts;
        isMoved = true;
    }

    if (isKeyDown(_window, GLFW_KEY_E))
    {
        _pos -= _upDir * _moveSpeed * ts;
        isMoved = true;
    }

    // camera rotations
    if (deltaMousePos.x != 0.0f || deltaMousePos.y != 0.0f)
    {
        float pitchDelta = deltaMousePos.y * _rotateSpped;
        float yawDelta = deltaMousePos.x * _rotateSpped;

        // TODO: figure out how quat works in graphics
        glm::quat q = glm::normalize(
            glm::cross(glm::angleAxis(-pitchDelta, _rightDir), glm::angleAxis(-yawDelta, glm::vec3(0.0f, 1.0f, 0.0f))));
        _viewDir = glm::rotate(q, _viewDir);

        isMoved = true;
    }

    // recalculate the view and proj matrices
    if (isMoved)
    {
        RecalViewMat();
    }
}

void Camera::RecalViewMat()
{
    //_viewMat = glm::lookAt(_pos, _pos + _viewDir, glm::vec3(0.0, 1.0, 0.0));
    _viewMat = glm::lookAt(_pos, _pos + _viewDir, _upDir);
    _invViewMat = glm::inverse(_viewMat);
}

void Camera::RecalProjMat()
{
    _projMat = glm::perspective(glm::radians(_fov), (float)_viewPortWidht / _viewPortHeight, _near, _far);
    _invProjMat = glm::inverse(_projMat);
}

} // namespace shift
