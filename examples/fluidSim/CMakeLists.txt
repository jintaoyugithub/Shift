# compile all vertex shaders
# quad
bgfx_compile_shaders(
  TYPE VERTEX
  SHADERS "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/quad_vs.sc"
  VARYING_DEF "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/quad_varying.def.sc"
  OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders/")

bgfx_compile_shaders(
  TYPE VERTEX
  SHADERS "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/quad_ws_vs.sc"
  VARYING_DEF "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/quad_varying.def.sc"
  OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders/")

# cube
bgfx_compile_shaders(
  TYPE VERTEX
  SHADERS "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/cube_vs.sc"
  VARYING_DEF "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/cube_varying.def.sc"
  OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders/")

# compile all fragment shaders
# quad 
bgfx_compile_shaders(
  TYPE FRAGMENT
  SHADERS "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/quad_fs.sc"
  VARYING_DEF "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/quad_varying.def.sc"
  OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders/")

# cube
bgfx_compile_shaders(
  TYPE FRAGMENT
  SHADERS "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/cube_fs.sc"
  VARYING_DEF "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/cube_varying.def.sc"
  OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders/")

# compile all fragment shaders
bgfx_compile_shaders(
  TYPE FRAGMENT
  SHADERS "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/quad_divergence_fs.sc"
  VARYING_DEF "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/quad_varying.def.sc"
  OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders/")

bgfx_compile_shaders(
  TYPE FRAGMENT
  SHADERS "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/quad_advect_fs.sc"
  VARYING_DEF "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/quad_varying.def.sc"
  OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders/")

bgfx_compile_shaders(
  TYPE FRAGMENT
  SHADERS "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/quad_project_fs.sc"
  VARYING_DEF "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/quad_varying.def.sc"
  OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders/")

# compile all compute shaders
bgfx_compile_shaders(
  TYPE COMPUTE 
  SHADERS "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/velocity_reset_cs.sc" 
  OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders/")

bgfx_compile_shaders(
  TYPE COMPUTE 
  SHADERS "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/velocity_addSource_cs.sc" 
  OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders/")

bgfx_compile_shaders(
  TYPE COMPUTE 
  SHADERS "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/velocity_advect_cs.sc" 
  OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders/")

bgfx_compile_shaders(
  TYPE COMPUTE 
  SHADERS "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/velocity_project_cs.sc" 
  OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders/")


add_executable(
  fluidSim
  fluidSim.cpp
  velocityField.cpp
  velocityFieldGrid.cpp
  velocityFieldCube.cpp

  # include all the shader files
  "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/quad_vs.sc"
  "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/quad_ws_vs.sc"
  "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/quad_fs.sc"
  "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/cube_vs.sc"
  "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/cube_fs.sc"
  "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/quad_advect_fs.sc"
  "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/quad_project_fs.sc"
  "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/quad_divergence_fs.sc"
  "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/velocity_reset_cs.sc" 
  "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/velocity_addSource_cs.sc" 
  "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/velocity_advect_cs.sc" 
  "${CMAKE_SOURCE_DIR}/shaders/examples/fluidSim/velocity_project_cs.sc" 
)

target_include_directories(
  fluidSim
  PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}/"
  "${CMAKE_SOURCE_DIR}/include/"
  "${CMAKE_SOURCE_DIR}/ext/bgfx.cmake/bgfx/3rdparty/"
  "${CMAKE_SOURCE_DIR}/ext/bgfx.cmake/bgfx/include/")

target_link_libraries(fluidSim PUBLIC shift_lib)
