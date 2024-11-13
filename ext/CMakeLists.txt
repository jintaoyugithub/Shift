# bgfx
set( BGFX_BUILD_TOOLS     ON  CACHE INTERNAL "" )
set( BGFX_BUILD_EXAMPLES  OFF CACHE INTERNAL "" )
set( BGFX_CUSTOM_TARGETS  OFF CACHE INTERNAL "" )
add_subdirectory( bgfx.cmake )
include( "./bgfx.cmake/cmake/bgfx/util/ConfigureDebugging.cmake" )

# glfw
set( GLFW_BUILD_EXAMPLES  OFF CACHE INTERNAL "" )
set( GLFW_BUILD_TESTS     OFF CACHE INTERNAL "" )
set( GLFW_BUILD_DOCS      OFF CACHE INTERNAL "" )
set( GLFW_INSTALL         OFF CACHE INTERNAL "" )
add_subdirectory( glfw )

# glm
set( GLM_TEST_ENABLE      OFF CACHE INTERNAL "" )
set( BUILD_SHARED_LIBS    OFF CACHE INTERNAL "" )
set( BUILD_STATIC_LIBS    OFF CACHE INTERNAL "" )
add_subdirectory( glm )

# imgui
add_library(
	imgui
	STATIC
	${CMAKE_CURRENT_LIST_DIR}/imgui/imgui.cpp
	${CMAKE_CURRENT_LIST_DIR}/imgui/imgui_demo.cpp
	${CMAKE_CURRENT_LIST_DIR}/imgui/imgui_draw.cpp
	${CMAKE_CURRENT_LIST_DIR}/imgui/imgui_widgets.cpp
)
target_include_directories( imgui PUBLIC ${CMAKE_CURRENT_LIST_DIR}/imgui )
set_target_properties( imgui PROPERTIES FOLDER "imgui" )

# spdlog
add_subdirectory( spdlog )

