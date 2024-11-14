# Shift Rendering Framework

## FAQ

1. Windows won't pop out in MacOS

- [github issue](https://github.com/LWJGL/lwjgl3/issues/619)

2. Where should I put my shader files?

3. Attribs which send to vertex shader must be in the following with the exact name, otherwise, it won't send anything to the vertex shader

- [vertex shader attribs](https://bkaradzic.github.io/bgfx/tools.html#vertex-shader-attributes)

4. Enable shader model 5 for d3d

In the file `bgfxToolUtils.cmake`, comment the line `list(APPEND PROFILES s_4_0)` inside the `function(bgfx_compile_shader)`, then the system will take shader model 5 as default!

5. Connection between gl_GlobalInvocationID and gl_FragCoord?