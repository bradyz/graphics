cmake_minimum_required(VERSION 2.8)
project(GLSL)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11 -g" )
set(CMAKE_FIND_LIBRARY_SUFFIXES "${CMAKE_FIND_LIBRARY_SUFFIXES}")

link_directories(
    "/usr/local/lib"
)

include_directories(
    "/usr/local/include"
    "${GLSL_SOURCE_DIR}"
    "${PROJECT_SOURCE_DIR}/include"
)

find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
link_libraries(${GLEW_LIBRARIES})

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})

find_package(JPEG REQUIRED)
include_directories(${JPEG_INCLUDE_DIR})

find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

# Boids Flocking Demo.
FILE(GLOB BOID_SRC ./src/boids.cpp ./src/**/*.cpp)
add_executable(boids ${BOID_SRC})

target_link_libraries(boids
                      ${OPENGL_gl_LIBRARY}
                      ${GLFW_LIBRARIES}
                      ${GLEW_LIBRARY}
                      ${JPEG_LIBRARIES}
                      ${LDFLAGS})

# Particle Physics Demo.
FILE(GLOB BALLS_SRC ./src/balls.cpp ./src/**/*.cpp)
add_executable(balls ${BALLS_SRC})
target_link_libraries(balls
                      ${OPENGL_gl_LIBRARY}
                      ${GLFW_LIBRARIES}
                      ${GLEW_LIBRARY}
                      ${JPEG_LIBRARIES}
                      ${LDFLAGS})

# Subdivision Algorithms Demo.
FILE(GLOB LOOP_SRC ./src/loop_subdivision.cpp ./src/**/*.cpp)
add_executable(loop_subdivision ${LOOP_SRC})
target_link_libraries(loop_subdivision
                      ${OPENGL_gl_LIBRARY}
                      ${GLFW_LIBRARIES}
                      ${GLEW_LIBRARY}
                      ${JPEG_LIBRARIES}
                      ${LDFLAGS})

# Cloth Simulation Demo.
FILE(GLOB CLOTH_SRC ./src/cloth.cpp ./src/**/*.cpp)
add_executable(cloth ${CLOTH_SRC})

target_link_libraries(cloth
                      ${OPENGL_gl_LIBRARY}
                      ${GLFW_LIBRARIES}
                      ${GLEW_LIBRARY}
                      ${JPEG_LIBRARIES}
                      ${LDFLAGS})

# Laplacian Mesh Smoothing.
FILE(GLOB LAPLACE_SRC ./src/laplace_smooth.cpp ./src/**/*.cpp)
add_executable(laplace_smooth ${LAPLACE_SRC})

target_link_libraries(laplace_smooth
                      ${OPENGL_gl_LIBRARY}
                      ${GLFW_LIBRARIES}
                      ${GLEW_LIBRARY}
                      ${JPEG_LIBRARIES}
                      ${LDFLAGS})
