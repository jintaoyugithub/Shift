set( LIB_NAME
    Shift
)

set( SOURCES
    appBase.cpp
)

add_library(
    ${LIB_NAME}
    STATIC
    ${SOURCES}
)

target_include_directories(
    ${LIB_NAME} 
    PUBLIC 
    "${CMAKE_SOURCE_DIR}/include/"
    "${CMAKE_SOURCE_DIR}/ext/bgfx.cmake/3rdparty/"
    "${CMAKE_SOURCE_DIR}/ext/bgfx.cmake/include/"
)

target_link_libraries(
    ${LIB_NAME}
    PUBLIC
    glm
    glfw
    bgfx
    bx
    bimg
)
