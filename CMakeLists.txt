cmake_minimum_required(VERSION 2.8)
project(GLSL)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11 -g" )
set(CMAKE_FIND_LIBRARY_SUFFIXES "${CMAKE_FIND_LIBRARY_SUFFIXES}")

if (APPLE)
  link_directories(
    "/usr/local/lib"
    "/opt/local/lib"
  )
elseif (UNIX)
  link_directories(
    "/usr/local/lib"
    "/opt/local/lib"
    "/lusr/lib"
    "/lusr/opt/glfw-3.1.2/lib"
  )
endif()

include_directories(
  "/usr/local/include"
  "/opt/local/include"
  "/lusr/include"
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

add_executable(assignment assignment.cc)

if (APPLE)
  set(LDFLAGS "")
elseif (UNIX)
  set(LDFLAGS
      "-lXext"
      "-lX11"
      "-ldl"
      "-lXfixes"
      "-lpthread"
      "-lXxf86vm"
      "-lXrandr"
      "-lXi"
      "-lrt"
      "-lm"
      "-lXinerama"
      "-lXcursor")
endif()

target_link_libraries(assignment
                      ${OPENGL_gl_LIBRARY}
                      ${GLFW_LIBRARIES}
                      ${GLEW_LIBRARY}
                      ${JPEG_LIBRARIES}
                      ${LDFLAGS}
)
