cmake_minimum_required(VERSION 3.10)
project(AstroSandbox)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Set Qt6 installation path
set(CMAKE_PREFIX_PATH "D:/QT/Installation/1st/6.9.0/mingw_64")

# Set library paths
set(GLM_DIR "${CMAKE_SOURCE_DIR}/include")
set(NLOHMANN_JSON_DIR "${CMAKE_SOURCE_DIR}/include/nlohmann")

# Find required packages
find_package(OpenGL REQUIRED)
find_package(Qt6 COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets REQUIRED)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${GLM_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${NLOHMANN_JSON_DIR}
)

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
    src/ResourceManager.cpp
    src/EngineBackend.cpp
    src/EngineConfig.cpp
    src/glad.c
    src/gui/MainWindow.cpp
    src/gui/OpenGLWidget.cpp
)

# Create executable
add_executable(astro-sim WIN32 ${SOURCES})

# Set include directories for the target
target_include_directories(astro-sim PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${GLM_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${NLOHMANN_JSON_DIR}
)

# Link libraries
target_link_libraries(astro-sim
    ${OPENGL_LIBRARIES}
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
)

# Add compiler warnings and optimizations
if(MSVC)
    target_compile_options(astro-sim PRIVATE /W4 /MP)
else()
    # Increase memory limit for MinGW
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--stack,33554432")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--stack,33554432")
    
    # Set environment variables for increased memory limit
    set(ENV{CFLAGS} "-fmax-errors=100 -Wl,--stack,33554432")
    set(ENV{CXXFLAGS} "-fmax-errors=100 -Wl,--stack,33554432")
    
    target_compile_options(astro-sim PRIVATE 
        -Wall 
        -Wextra 
        -Wpedantic
        -fmax-errors=100
        -O2
        -pipe
        -fno-omit-frame-pointer
        -fno-keep-inline-dllexport
        -fno-keep-inline-functions
        -ffunction-sections
        -fdata-sections
        -fexceptions
        -frtti
    )
    
    # Suppress pedantic warnings for glad.c
    set_source_files_properties(src/glad.c PROPERTIES COMPILE_FLAGS "-Wno-pedantic")
endif()

# Enable parallel compilation with reduced number of jobs
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe")

# Set build type to Release for better performance
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Set number of parallel jobs
set(CMAKE_BUILD_PARALLEL_LEVEL 2) 