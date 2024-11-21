# Shift Rendering Framework

## TODO Lists

### 2024.11.20

- [x] Add UI support!
- [ ] Refactor the code, to make right mouse click to `add density` and left mouse click to `add velocity`

**Note**: When mouse clicks then add source, so you should dispatch the add source compute shader when the mouse is clicked, may be that can solve the bug of `multiple clicks after releasing the mouse`.

- [ ] Add the boundary code
- [ ] Change the velocity calculation from `collocated grid` to `staggered grid` 
- [ ] Calculate divergence, instead of storing velocity at the center of each cell, stroing the velocity at the edge cell.

**Note**: check the function `ProjectGaussSeidel` in [FluidSim](https://github.com/hughperkins/UnityFluidSim-pub)

- [ ] Use horizontal component and vertical component of the velocity instead of using whole vec2
- [ ] Finish the velocity component first, including `add velocity`, `advect` and `project`

### 2024.11.21

- [ ] Need to implement the glfw mouse button callback to make sure glfw send the mouse event to the imgui


## Bugs & Questions

### 2024.11.18

- [ ] Multiple click of mouse when try to release the mouse left button
- [ ] The function `VelocityProject` display black right now

### 2024.11.20

- [ ] 我现在通过一个`radius`的变量来控制add velocity的范围，如何让鼠标活动影响的程度从重心向radius减小？
- [ ] 而且对于速度场来说，在增加速度场的时候，需要使用`+=` 吗？For example

### 2024.11.21

- [ ] Imgui window and glfw window can not communicate, which means they are not in the same context

```cpp
curVelocityField[index] += vec2(xVel, yVel) + prevVelocityField[index] * deltaTime;
// or
curVelocityField[index] = vec2(xVel, yVel); 
```

- [ ] May be I don't need diffuse, I just `add noise`, it can also save me some performance

So the current steps will be:

- For velocity
add source -> project -> advect

- For density
add source -> add noise -> advect


## Reminder

1. Change the source code of `imgui/backends/imgui_impl_glfw.h/cpp` because I use the `dear-imgui` in the `3rdparty` of bgfx

2. Change the `CMakeLists.txt` of the bgfx.cmake to enable `shader model 5` 

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

`Update on 2024.11.17`:

Better only use `float`, especially when you gonna use the full storage of the unifroms, and also the value that you send to the uniform should also be `float`, otherwise it may not be sent to the GPU, for example:

```cpp
struct data {
    // NO! int32_t, better all floats
    float x;
    float y;
    float z;
    float w;
};

void init(data* _data) {
    _data = 0.0f; // the value here should also be float, no int, uint etc.
    // ...
}
```

7. Framerate issues

I'm not sure why the frame rate in this project changes with the display's maximum refresh rate. For example, with a 60Hz monitor, the frame rate is capped at 60, but when I switch to a 140Hz monitor, it reaches 140. Disabling V-Sync in the NVIDIA control panel under 3D settings unlocks the frame rate limit.

8. `.sh` file doesn't update after modified it

You should update the shader, for example, add some comments, in order to make the `bgfx` rebuild the shader.
