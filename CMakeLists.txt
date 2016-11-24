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

if (APPLE)
  find_library(COCOA_LIBRARY Cocoa REQUIRED)
endif(APPLE)

FILE(GLOB BOID_SRC *.cpp)
list(REMOVE_ITEM BOID_SRC ${CMAKE_CURRENT_SOURCE_DIR}/balls.cpp)
list(REMOVE_ITEM BOID_SRC ${CMAKE_CURRENT_SOURCE_DIR}/catmull.cpp)
add_executable(boids ${BOID_SRC})

FILE(GLOB BALLS_SRC *.cpp)
list(REMOVE_ITEM BALLS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/boids.cpp)
list(REMOVE_ITEM BALLS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/catmull.cpp)
add_executable(balls ${BALLS_SRC})

FILE(GLOB CATMULL_SRC *.cpp)
list(REMOVE_ITEM CATMULL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/boids.cpp)
list(REMOVE_ITEM CATMULL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/balls.cpp)
add_executable(catmull ${CATMULL_SRC})

set(LDFLAGS "")

target_link_libraries(boids
                      ${OPENGL_gl_LIBRARY}
                      ${GLFW_LIBRARIES}
                      ${GLEW_LIBRARY}
                      ${JPEG_LIBRARIES}
                      ${LDFLAGS})

target_link_libraries(balls
                      ${OPENGL_gl_LIBRARY}
                      ${GLFW_LIBRARIES}
                      ${GLEW_LIBRARY}
                      ${JPEG_LIBRARIES}
                      ${LDFLAGS})

target_link_libraries(catmull
                      ${OPENGL_gl_LIBRARY}
                      ${GLFW_LIBRARIES}
                      ${GLEW_LIBRARY}
                      ${JPEG_LIBRARIES}
                      ${LDFLAGS})
