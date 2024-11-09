# compile all vertex shaders
bgfx_compile_shaders(
	TYPE VERTEX
	SHADERS "${CMAKE_SOURCE_DIR}/shaders/common/quad_vs.sc"
	VARYING_DEF "${CMAKE_SOURCE_DIR}/shaders/common/quad_varying.def.sc"
	OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders/"
)

# compile all fragment shaders
bgfx_compile_shaders(
	TYPE FRAGMENT
	SHADERS "${CMAKE_SOURCE_DIR}/shaders/common/quad_fs.sc"
	VARYING_DEF "${CMAKE_SOURCE_DIR}/shaders/common/quad_varying.def.sc"
	OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders/"
)

# compile all compute shaders
bgfx_compile_shaders(
	TYPE COMPUTE
	SHADERS "${CMAKE_SOURCE_DIR}/shaders/examples/computeShader/buffer_cs.sc"
	VARYING_DEF "${CMAKE_SOURCE_DIR}/shaders/common/quad_varying.def.sc"
	OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders/"
)

bgfx_compile_shaders(
	TYPE COMPUTE
	SHADERS "${CMAKE_SOURCE_DIR}/shaders/examples/computeShader/image_cs.sc"
	VARYING_DEF "${CMAKE_SOURCE_DIR}/shaders/common/quad_varying.def.sc"
	OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders/"
)


add_executable(computeShader 
    computeShader.cpp
    # include all the shader files
	"${CMAKE_SOURCE_DIR}/shaders/common/quad_vs.sc"
	"${CMAKE_SOURCE_DIR}/shaders/common/quad_fs.sc"
	"${CMAKE_SOURCE_DIR}/shaders/examples/computeShader/image_cs.sc"
	"${CMAKE_SOURCE_DIR}/shaders/examples/computeShader/buffer_cs.sc"
)

target_include_directories(
    computeShader
    PUBLIC 
    "${CMAKE_SOURCE_DIR}/include/"
    "${CMAKE_SOURCE_DIR}/ext/bgfx.cmake/bgfx/3rdparty/"
    "${CMAKE_SOURCE_DIR}/ext/bgfx.cmake/bgfx/include/"
)

target_link_libraries(
    computeShader
    PUBLIC
    shift_lib
)