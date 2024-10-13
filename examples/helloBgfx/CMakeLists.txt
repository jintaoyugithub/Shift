add_executable(helloBfgx helloBgfx.cpp)

target_include_directories(
    helloBfgx
    PUBLIC 
    "${CMAKE_SOURCE_DIR}/include/"
    "${CMAKE_SOURCE_DIR}/ext/bgfx.cmake/3rdparty/"
    "${CMAKE_SOURCE_DIR}/ext/bgfx.cmake/include/"
)

target_link_libraries(
    helloBfgx
    PUBLIC
    shift_lib
)
