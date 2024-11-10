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
