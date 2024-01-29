cmake_minimum_required(VERSION 3.0)
project(glrt)

add_subdirectory(glfw)
add_subdirectory(glmath)

include_directories(glfw/include glmath/include include)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/source/*.c)

add_executable(glrt ${SOURCES})

file(GLOB SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.glsl)

if(SHADERS)
	file(COPY ${SHADERS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

target_link_libraries(glrt glfw glmath)