cmake_minimum_required(VERSION 3.0)
project(DemoProject)

# Add the executable with all your source files
add_executable(demo demo.cpp glad.c shader.cpp points.cpp shapes.cpp point.cpp tracker.cpp value_tracker.cpp)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find packages
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenCL REQUIRED)

# Platform-specific libraries
find_package(X11 REQUIRED)
find_package(TBB REQUIRED)
set(PLATFORM_LIBS ${X11_LIBRARIES} "tbb" "Xxf86vm" "Xrandr" "Xi")

# Link against the required libraries
target_link_libraries(demo
    OpenGL::GL
    glfw
    ${CMAKE_THREAD_LIBS_INIT}
    ${PLATFORM_LIBS}
    dl
    OpenCL::OpenCL
)

# Optionally, set the compiler to use the -g flag (debug information)
set(CMAKE_BUILD_TYPE Debug)

# Testing
enable_testing()
add_test(NAME ShaderTest COMMAND demo vertexShader.glsl fragmentShader.glsl)
