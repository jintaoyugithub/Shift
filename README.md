# Shift Rendering Framework

## TODO Lists

### 2024.11.20

- [x] Add UI support!
- [x] Refactor the code, to make right mouse click to `add density` and left mouse click to `add velocity`

**Note**: When mouse clicks then add source, so you should dispatch the add source compute shader when the mouse is clicked, may be that can solve the bug of `multiple clicks after releasing the mouse`.

- [x] Add the boundary code
- [x] Change the velocity calculation from `collocated grid` to `staggered grid` 
- [x] Calculate divergence, instead of storing velocity at the center of each cell, stroing the velocity at the edge cell.

**Note**: check the function `ProjectGaussSeidel` in [FluidSim](https://github.com/hughperkins/UnityFluidSim-pub)

- [x] Use horizontal component and vertical component of the velocity instead of using whole vec2
- [ ] Finish the velocity component first, including `add velocity`, `advect` and `project`
    - [x] add velocity
    - [x] project
    - [ ] advect

### 2024.11.21

- [x] Need to implement the glfw mouse button callback to make sure glfw send the mouse event to the imgui

### 2024.11.24

- [ ] interpolate the color

### 2024.11.25

- [x] finish `advect` function, we can run interpolate in `afvect` first, then add advect
- [ ] add camera movement
- [ ] interpolate the velocity after `add source`

### 2024.12.03

- [ ] volumetric data of **isFluid** and display it

actually **Velocity Field in 3D** can be also considered as the volumetric data

- [ ] Laid down the 2 dimension one, and add sphere to interact.

## Bugs & Questions

### 2024.11.18

- [x] Multiple click of mouse when try to release the mouse left button

`Posible solution`: reset the mouse pos which sent to the compute shader to 0 or somevalue after click, 因为这样就不会导致传给compute shader的鼠标位置的值一直固定在我松开鼠标的那个位置。

- [x] The function `VelocityProject` display black right now

### 2024.11.20

- [ ] 我现在通过一个`radius`的变量来控制add velocity的范围，如何让鼠标活动影响的程度从重心向radius减小？
- [ ] 而且对于速度场来说，在增加速度场的时候，需要使用`+=` 吗？For example

```cpp
curVelocityField[index] += vec2(xVel, yVel) + prevVelocityField[index] * deltaTime;
// or
curVelocityField[index] = vec2(xVel, yVel); 
```

### 2024.11.21

- [ ] Imgui window and glfw window can not communicate, which means they are not in the same context

### 2024.11.26

- [x] 流体模拟在经过advect之后，会导致全局的velocity field都朝向左下角，也就是x负，y正（向右为x正，向下为y正）
- [x] project的correction太大会导致整个velocity field非常混乱
- [x] 右和上边界是有问题的

- [ ] May be I don't need diffuse, I just `add noise`, it can also save me some performance

So the current steps will be:

- For velocity
add source -> project -> advect

- For density
add source -> add noise -> advect

### 2024.11.27

- [ ] advect的方向会往比正常方向偏左，检查一下advect中的函数


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

**6. uniforms**

- Uniform structure

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

- Uniform data

the value you sent to the uniform must be **represent as float**, which means for example:

```cpp
_data = 0.0f; // Correct
_data = 0.0;  // false, will be consider as double
```

7. Framerate issues

I'm not sure why the frame rate in this project changes with the display's maximum refresh rate. For example, with a 60Hz monitor, the frame rate is capped at 60, but when I switch to a 140Hz monitor, it reaches 140. Disabling V-Sync in the NVIDIA control panel under 3D settings unlocks the frame rate limit.

8. `.sh` file doesn't update after modified it

You should update the shader, for example, add some comments, in order to make the `bgfx` rebuild the shader.

9. When change some files that are not shader code and are included more than one file, for every shader that include that file, you should change that shader code a little bit to make sure recompile the shader again to apply the changeds.

For example in the `fluid simulation shader folder`, there is a file call `velocity_uniforms.sh` which store all the definition of the uniforms, if you change that file, you need to add a tiny change, for instance **add a comment**, to make sure the shader will be recompiled.
