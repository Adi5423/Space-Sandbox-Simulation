cmake_minimum_required(VERSION 3.10)
project(AstroSandbox)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set library paths
set(GLFW_DIR "${CMAKE_SOURCE_DIR}/lib")
set(GLM_DIR "${CMAKE_SOURCE_DIR}/include")
set(NLOHMANN_JSON_DIR "${CMAKE_SOURCE_DIR}/include/nlohmann")
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/external/imgui")

# Find required packages
find_package(OpenGL REQUIRED)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${GLM_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${NLOHMANN_JSON_DIR}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
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
    src/ResourceManager.cpp
    src/EngineBackend.cpp
    src/EngineConfig.cpp
    src/glad.c
    src/imgui/ImGuiManager.cpp
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# ImGui source files
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Create executable
add_executable(astro-sim WIN32 ${SOURCES} ${IMGUI_SOURCES})

# Set include directories for the target
target_include_directories(astro-sim PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${GLM_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${NLOHMANN_JSON_DIR}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# Link libraries
target_link_libraries(astro-sim
    ${OPENGL_LIBRARIES}
    ${CMAKE_SOURCE_DIR}/lib/libglfw3.a
    gdi32
)

# Add compiler warnings and optimizations
if(MSVC)
    target_compile_options(astro-sim PRIVATE /W4 /MP)
else()
    target_compile_options(astro-sim PRIVATE 
        -Wall 
        -Wextra 
        -Wpedantic
        -fmax-errors=100
        -O2
        -pipe
        -fno-omit-frame-pointer
    )
    # Suppress pedantic warnings for glad.c
    set_source_files_properties(src/glad.c PROPERTIES COMPILE_FLAGS "-Wno-pedantic")
    # Set environment variable for increased memory limit
    set(ENV{CFLAGS} "-fmax-errors=100")
    set(ENV{CXXFLAGS} "-fmax-errors=100")
endif()

# Enable parallel compilation
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe") 