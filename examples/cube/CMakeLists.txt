# compile all vertex shaders
bgfx_compile_shaders(
	TYPE VERTEX
	SHADERS "${CMAKE_SOURCE_DIR}/shaders/examples/cube/vs_cube.sc"
	VARYING_DEF "${CMAKE_SOURCE_DIR}/shaders/examples/cube/varying.def.sc"
	OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders/"
)

# compile all fragment shaders
bgfx_compile_shaders(
	TYPE FRAGMENT
	SHADERS "${CMAKE_SOURCE_DIR}/shaders/examples/cube/fs_cube.sc"
	VARYING_DEF "${CMAKE_SOURCE_DIR}/shaders/examples/cube/varying.def.sc"
	OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders/"
)


add_executable(cube 
    cube.cpp
    # include all the shader files
    "${CMAKE_SOURCE_DIR}/shaders/examples/cube/vs_cube.sc"
    "${CMAKE_SOURCE_DIR}/shaders/examples/cube/fs_cube.sc"
)

target_include_directories(
    cube
    PUBLIC 
    "${CMAKE_SOURCE_DIR}/include/"
    "${CMAKE_SOURCE_DIR}/ext/bgfx.cmake/bgfx/3rdparty/"
    "${CMAKE_SOURCE_DIR}/ext/bgfx.cmake/bgfx/include/"
)

target_link_libraries(
    cube
    PUBLIC
    shift_lib
)