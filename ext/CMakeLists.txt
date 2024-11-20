# bgfx
set(BGFX_BUILD_TOOLS
    ON
    CACHE INTERNAL "")
set(BGFX_BUILD_EXAMPLES
    OFF
    CACHE INTERNAL "")
set(BGFX_CUSTOM_TARGETS
    OFF
    CACHE INTERNAL "")
add_subdirectory(bgfx.cmake)
include("./bgfx.cmake/cmake/bgfx/util/ConfigureDebugging.cmake")

# glfw
set(GLFW_BUILD_EXAMPLES
    OFF
    CACHE INTERNAL "")
set(GLFW_BUILD_TESTS
    OFF
    CACHE INTERNAL "")
set(GLFW_BUILD_DOCS
    OFF
    CACHE INTERNAL "")
set(GLFW_INSTALL
    OFF
    CACHE INTERNAL "")
add_subdirectory(glfw)

# glm
set(GLM_TEST_ENABLE
    OFF
    CACHE INTERNAL "")
set(BUILD_SHARED_LIBS
    OFF
    CACHE INTERNAL "")
set(BUILD_STATIC_LIBS
    OFF
    CACHE INTERNAL "")
add_subdirectory(glm)

# imgui
add_library(
  bgfx-imgui STATIC
  "${CMAKE_SOURCE_DIR}/ext/bgfx.cmake/bgfx/3rdparty/dear-imgui/imgui.cpp"
  "${CMAKE_SOURCE_DIR}/ext/bgfx.cmake/bgfx/3rdparty/dear-imgui/imgui_demo.cpp"
  "${CMAKE_SOURCE_DIR}/ext/bgfx.cmake/bgfx/3rdparty/dear-imgui/imgui_draw.cpp"
  "${CMAKE_SOURCE_DIR}/ext/bgfx.cmake/bgfx/3rdparty/dear-imgui/imgui_tables.cpp"
  "${CMAKE_SOURCE_DIR}/ext/bgfx.cmake/bgfx/3rdparty/dear-imgui/imgui_widgets.cpp"
)

target_include_directories(
  bgfx-imgui
  PUBLIC "${CMAKE_SOURCE_DIR}/ext/bgfx.cmake/bgfx/3rdparty/"
         "${CMAKE_SOURCE_DIR}/ext/bgfx.cmake/bgfx/include/"
         "${CMAKE_SOURCE_DIR}/ext/bgfx.cmake/bx/include/"
         "${CMAKE_SOURCE_DIR}/ext/bgfx.cmake/bimg/include/")

target_link_libraries(bgfx-imgui PUBLIC bgfx bx bimg)

set_target_properties(bgfx-imgui PROPERTIES FOLDER "imgui")

# spdlog
add_subdirectory(spdlog)
