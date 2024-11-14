# Shift Rendering Framework

## FAQ

1. Windows won't pop out in MacOS

- [github issue](https://github.com/LWJGL/lwjgl3/issues/619)

2. Where should I put my shader files?

3. Attribs which send to vertex shader must be in the following with the exact name, otherwise, it won't send anything to the vertex shader

- [vertex shader attribs](https://bkaradzic.github.io/bgfx/tools.html#vertex-shader-attributes)

4. Enable shader model 5 for d3d

In the file `bgfxToolUtils.cmake`, comment the line `list(APPEND PROFILES s_4_0)` inside the `function(bgfx_compile_shader)`, then the system will take shader model 5 as default!

5. Connection between mouse position and gl_FragCoord?

The origin of gl_FragCoord is bottom left, however the origin of glfwGetCursorPos() is top left

6. uniforms structure

If you want to send a struct uniform, for example:

```cpp
struct uParams {
    float mousePosX;
    float mousePosY;
    int32_t bufferWidth;
    //...
}
```

Make sure you are using `float` or `int32_t` which means each variable should occupy the same amount of memory. `double` is not allowed, same as `int16_t`, `bool` etc.

**Note**: why float? Cause bgfx force you to use float as the standard variable type of a uniform.

7. Framerate issues

I'm not sure why the frame rate in this project changes with the display's maximum refresh rate. For example, with a 60Hz monitor, the frame rate is capped at 60, but when I switch to a 140Hz monitor, it reaches 140. Disabling V-Sync in the NVIDIA control panel under 3D settings unlocks the frame rate limit.

