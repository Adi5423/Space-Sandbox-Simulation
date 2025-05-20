cmake_minimum_required(VERSION 3.10)
project(AstroSandbox)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set library paths
set(GLFW_DIR "${CMAKE_SOURCE_DIR}/lib")
set(GLM_DIR "${CMAKE_SOURCE_DIR}/include")

# Find required packages
find_package(OpenGL REQUIRED)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${GLM_DIR}
    ${OPENGL_INCLUDE_DIR}
)

# Link directories
link_directories(${GLFW_DIR})

# Source files
set(SOURCES
    src/main.cpp
    src/Renderer.cpp
    src/World.cpp
    src/Mesh.cpp
    src/Body.cpp
    src/Simulator.cpp
    src/Grid.cpp
    src/TextRenderer.cpp
    src/glad.c
)

# Create executable
add_executable(astro-sim ${SOURCES})

# Link libraries
target_link_libraries(astro-sim
    ${OPENGL_LIBRARIES}
    ${CMAKE_SOURCE_DIR}/lib/libglfw3.a
    gdi32
)

# Add compiler warnings
if(MSVC)
    target_compile_options(astro-sim PRIVATE /W4)
else()
    target_compile_options(astro-sim PRIVATE -Wall -Wextra -Wpedantic)
endif() 