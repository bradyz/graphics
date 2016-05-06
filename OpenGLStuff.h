#ifndef OPENGLSTUFF_H
#define OPENGLSTUFF_H

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

extern glm::vec4 LIGHT_POSITION;

extern const float kNear;
extern const float kFar;
extern const float kFov;

extern float camera_distance;

extern glm::vec3 up;
extern glm::vec3 look;
extern glm::vec3 tangent;
extern glm::mat3 orientation;

extern glm::vec3 eye;
extern glm::vec3 center;

extern glm::mat4 view_matrix;
extern glm::mat4 projection_matrix;

extern const std::string window_title;

extern const int WIDTH;
extern const int HEIGHT;

extern bool timePaused;
extern bool showWire;
extern bool showFloor;

enum {
  kVertexBuffer,
  kIndexBuffer,
  kVertexNormalBuffer,
  kNumVbos
};

enum {
  kFloorVao,
  kLineSegVao,
  kPhongVao,
  kShadowVao,
  kWireVao,
  kNumVaos
};

extern GLuint array_objects[kNumVaos];
extern GLuint buffer_objects[kNumVaos][kNumVbos];

void initOpenGL();
bool keepLoopingOpenGL();
void cleanupOpenGL();
void endLoopOpenGL();

GLuint setupShader (const char* shaderName, GLenum shaderType);

extern const char* OpenGlErrorToString(GLenum error);

#define CHECK_SUCCESS(x) \
  if (!(x)) {            \
    glfwTerminate();     \
    exit(EXIT_FAILURE);  \
  }

#define CHECK_GL_SHADER_ERROR(id)                                           \
  {                                                                         \
    GLint status = 0;                                                       \
    GLint length = 0;                                                       \
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);                          \
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);                         \
    if (!status) {                                                          \
      string log(length, 0);                                                \
      glGetShaderInfoLog(id, length, nullptr, &log[0]);                     \
      cerr << "Line :" << __LINE__ << " OpenGL Shader Error: Log = \n"      \
                << &log[0];                                                 \
      glfwTerminate();                                                      \
      exit(EXIT_FAILURE);                                                   \
    }                                                                       \
  }

#define CHECK_GL_PROGRAM_ERROR(id)                                          \
  {                                                                         \
    GLint status = 0;                                                       \
    GLint length = 0;                                                       \
    glGetProgramiv(id, GL_LINK_STATUS, &status);                            \
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);                        \
    if (!status) {                                                          \
      string log(length, 0);                                                \
      glGetProgramInfoLog(id, length, nullptr, &log[0]);                    \
      cerr << "Line :" << __LINE__ << " OpenGL Program Error: Log = \n"     \
                << &log[0];                                                 \
      glfwTerminate();                                                      \
      exit(EXIT_FAILURE);                                                   \
    }                                                                       \
  }

#define CHECK_GL_ERROR(statement)                                           \
  {                                                                         \
    { statement; }                                                          \
    GLenum error = GL_NO_ERROR;                                             \
    if ((error = glGetError()) != GL_NO_ERROR) {                            \
      cerr << "Line :" << __LINE__ << " OpenGL Error: code  = " << error    \
                << " description =  " << OpenGlErrorToString(error);        \
      glfwTerminate();                                                      \
      exit(EXIT_FAILURE);                                                   \
    }                                                                       \
  }

#endif
