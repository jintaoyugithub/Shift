add_executable(imguiExample
    imgui.cpp
    # include all the shader files
)

target_include_directories(
    imguiExample
    PUBLIC 
    "${CMAKE_SOURCE_DIR}/include/"
    "${CMAKE_SOURCE_DIR}/ext/bgfx.cmake/bgfx/3rdparty/"
    "${CMAKE_SOURCE_DIR}/ext/bgfx.cmake/bgfx/include/"
)

target_link_libraries(
    imguiExample
    PUBLIC
    shift_lib
)