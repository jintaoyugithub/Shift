add_executable(cube cube.cpp)

target_include_directories(
    cube
    PUBLIC 
    "${CMAKE_SOURCE_DIR}/include/"
    "${CMAKE_SOURCE_DIR}/ext/bgfx.cmake/3rdparty/"
    "${CMAKE_SOURCE_DIR}/ext/bgfx.cmake/include/"
)

target_link_libraries(
    cube
    PUBLIC
    shift_lib
)