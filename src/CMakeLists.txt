set(LIB_NAME shift_lib)

set(SOURCES
    appBase.cpp 
    appBaseGLFW.cpp 
    "utils/bgfx.cpp" 
    "utils/glfw.cpp"
    # "utils/imgui.cpp"
    "utils/common.cpp"
    "utils/camera.cpp"
)

add_library(${LIB_NAME} STATIC ${SOURCES})

target_include_directories(
  ${LIB_NAME}
  PUBLIC "${CMAKE_SOURCE_DIR}/include/"
         "${CMAKE_SOURCE_DIR}/shaders/"
         "${CMAKE_SOURCE_DIR}/ext/bgfx.cmake/bgfx/3rdparty/"
         "${CMAKE_SOURCE_DIR}/ext/bgfx.cmake/bgfx/include/"
         "${CMAKE_SOURCE_DIR}/ext/bgfx.cmake/bx/include/"
         "${CMAKE_SOURCE_DIR}/ext/bgfx.cmake/bimg/include/"
         "${CMAKE_SOURCE_DIR}/ext/spdlog/include/"
         # "${CMAKE_SOURCE_DIR}/ext/imgui/"
         "${CMAKE_SOURCE_DIR}/ext/gleq/")

target_link_libraries(${LIB_NAME} PUBLIC glm glfw bgfx bx bimg spdlog)
