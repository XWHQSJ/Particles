cmake_minimum_required(VERSION 3.16)
project(Particles VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Options ---
option(USE_MODERN_GL "Enable modern OpenGL 3.3 core renderer" ON)
option(BUILD_TESTS "Build physics unit tests" ON)

# --- GLFW via FetchContent ---
include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# --- Find OpenGL ---
find_package(OpenGL REQUIRED)

# --- Common library ---
add_library(particle_common STATIC
    common/particle.cpp
    common/app.cpp
    common/texture.cpp
)
target_include_directories(particle_common PUBLIC common)
target_link_libraries(particle_common PUBLIC glfw OpenGL::GL)

if(APPLE)
    target_link_libraries(particle_common PUBLIC
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
    target_compile_definitions(particle_common PUBLIC GL_SILENCE_DEPRECATION)
endif()

if(USE_MODERN_GL)
    target_sources(particle_common PRIVATE common/renderer_modern.cpp)
    target_compile_definitions(particle_common PUBLIC USE_MODERN_GL=1)
endif()

# --- Demo executables ---
set(DEMOS
    particles
    butterfly
    starCross
    stars
    torchLight
    X-Men
    fire
    fountain
    snow
    galaxy
    explosion
)

foreach(demo ${DEMOS})
    add_executable(${demo} demos/${demo}.cpp)
    target_link_libraries(${demo} PRIVATE particle_common)
    # Set working directory to project root for texture loading
    set_target_properties(${demo} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/demos"
    )
endforeach()

# --- Tests ---
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
