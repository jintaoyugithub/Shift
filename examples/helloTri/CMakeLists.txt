add_executable(helloTri helloTri.cpp)

target_include_directories(
    helloTri
    PUBLIC 
    "${CMAKE_SOURCE_DIR}/include/"
    "${CMAKE_SOURCE_DIR}/ext/bgfx.cmake/3rdparty/"
    "${CMAKE_SOURCE_DIR}/ext/bgfx.cmake/include/"
)

target_link_libraries(
    helloTri
    PUBLIC
    Shift
)
