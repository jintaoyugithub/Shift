#pragma once

#include "appBase.hpp"
#include <utils/common.hpp>

namespace shift
{

struct MouseButton
{
    // Note that enum class is a strong type
    // It's not allowed implicitly transfer, for example, to int
    // enum class type
    enum Enum
    {
        None,
        Left,
        Middle,
        Right,

        Count
    };
};

struct MouseState
{
    MouseState() : _x(0), _y(0), _z(0)
    {
        for (auto ii = 0; ii < shift::MouseButton::Count; ++ii)
        {
            _buttons[ii] = shift::MouseButton::None;
        }
    }

    double _x;
    double _y;
    int32_t _z;
    uint8_t _buttons[shift::MouseButton::Count];
};

class AppBaseGLFW : public shift::AppBase
{
  private:
    virtual bool bgfxInit(int _argc, const char **_argv) final override;
    virtual bool windowInit(uint32_t width, uint32_t height) override;
    virtual bool uiInit() override;

  public:
    AppBaseGLFW(const char *name, const char *description, const char *url);
    ~AppBaseGLFW() override;
    virtual void init(int _argc, const char **_argv, uint32_t width, uint32_t height) override;
    virtual bool update() override;
    virtual void shutdown() override;
    virtual void run(int _argc, const char **_argv) override;

    inline uint32_t getWidth() const
    {
        return _width;
    };
    inline uint32_t getHeight() const
    {
        return _height;
    };

  protected:
    GLFWwindow *_window;
    shift::MouseState _mouseState;

  private:
    uint32_t _width;
    uint32_t _height;
    const char *_title;
};

} // namespace shift
