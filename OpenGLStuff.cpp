#include <string>
#include <iostream>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "Program.h"
#include "OpenGLStuff.h"
#include "RandomUtils.h"

using namespace std;

enum {
  kMouseModeCamera,
  kNumMouseModes
};

GLuint array_objects[kNumVaos];
GLuint buffer_objects[kNumVaos][kNumVbos];

int current_mouse_mode = 0;

glm::vec4 LIGHT_POSITION = glm::vec4(10.0f, 10.0f, 10.0f, 1.0f);

const float kNear = 0.0001f;
const float kFar = 1000.0f;
const float kFov = 45.0f;
float camera_distance = 15.0f;

glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 look = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 tangent = glm::vec3(1.0f, 0.0f, 0.0f);
glm::mat3 orientation = glm::mat3(tangent, up, look);

glm::vec3 eye = glm::vec3(15.0f, 15.0f, camera_distance - 2.0f);
glm::vec3 center = eye + camera_distance * look;

glm::mat4 view_matrix;
glm::mat4 projection_matrix;

const std::string window_title = "Collision Detection";

const int WIDTH = 800;
const int HEIGHT = 600;
int window_width = WIDTH;
int window_height = HEIGHT;

float last_x = 0.0f, last_y = 0.0f, current_x = 0.0f, current_y = 0.0f;
bool drag_state = false;
bool button_press = false;
int current_button = -1;
const float pan_speed = 0.1f;
const float roll_speed = 0.1f;
const float rotation_speed = 0.05f;
const float zoom_speed = 0.1f;

bool fps_mode = false;

bool timePaused = false;
bool showWire = false;

GLFWwindow* window;

const char* OpenGlErrorToString(GLenum error) {
  switch (error) {
    case GL_NO_ERROR:
      return "GL_NO_ERROR";
      break;
    case GL_INVALID_ENUM:
      return "GL_INVALID_ENUM";
      break;
    case GL_INVALID_VALUE:
      return "GL_INVALID_VALUE";
      break;
    case GL_INVALID_OPERATION:
      return "GL_INVALID_OPERATION";
      break;
    case GL_OUT_OF_MEMORY:
      return "GL_OUT_OF_MEMORY";
      break;
    default:
      return "Unknown Error";
      break;
  }
  return "Unicorns Exist";
}

GLuint setupShader (const char* shaderName, GLenum shaderType) {
  GLuint shaderId = 0;
  CHECK_GL_ERROR(shaderId = glCreateShader(shaderType));
  CHECK_GL_ERROR(glShaderSource(shaderId, 1, &shaderName, nullptr));
  glCompileShader(shaderId);
  CHECK_GL_SHADER_ERROR(shaderId);
  return shaderId;
}

void ErrorCallback (int error, const char* description) {
  cerr << "GLFW Error: " << description << "\n";
}

void KeyCallback (GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
  else if (action != GLFW_RELEASE) {
    if (key == GLFW_KEY_W) {
      if (fps_mode)
        eye -= zoom_speed * look;
      else
        camera_distance = fmax(0.1f, camera_distance - zoom_speed);
    } 
    else if (key == GLFW_KEY_S) {
      if (fps_mode)
        eye += zoom_speed * look;
      else
        camera_distance += zoom_speed;
    } 
    else if (key == GLFW_KEY_A) {
      if (fps_mode)
        eye -= pan_speed * tangent;
      else
        center -= pan_speed * tangent;
    } 
    else if (key == GLFW_KEY_D) {
      if (fps_mode)
        eye += pan_speed * tangent;
      else
        center += pan_speed * tangent;
    } 
    else if (key == GLFW_KEY_C) {
      fps_mode = !fps_mode;
    } 
    else if (key == GLFW_KEY_M) {
      showWire = !showWire;
    } 
    else if (key == GLFW_KEY_T) {
      timePaused = !timePaused;
    } 
  }
}

void MousePosCallback(GLFWwindow* window, double mouse_x, double mouse_y) {
  last_x = current_x;
  last_y = current_y;
  current_x = mouse_x;
  current_y = window_height - mouse_y;

  float delta_x = current_x - last_x;
  float delta_y = current_y - last_y;

  if (delta_x * delta_x + delta_y * delta_y < 1e-15)
    return;

  glm::vec3 mouse_direction = glm::normalize(glm::vec3(delta_x, delta_y, 0.0f));

  glm::vec3 mouse = glm::vec3(mouse_direction.y, -mouse_direction.x, 0.0f);
  glm::vec3 axis = glm::normalize(orientation * mouse);

  if (drag_state && current_button == GLFW_MOUSE_BUTTON_LEFT) {
    if (current_mouse_mode == kMouseModeCamera) {
      orientation = glm::mat3(glm::rotate(rotation_speed, axis) * glm::mat4(orientation));
      look = glm::column(orientation, 2);
      tangent = glm::cross(up, look);
    }
  }
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
  drag_state = (action == GLFW_PRESS);
  current_button = button;
}

void initOpenGL () {
  if (!glfwInit())
    exit(EXIT_FAILURE);

  glfwSetErrorCallback(ErrorCallback);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);

  window = glfwCreateWindow(window_width, window_height,
                            &window_title[0], nullptr, nullptr);

  CHECK_SUCCESS(window != nullptr);

  glfwMakeContextCurrent(window);
  glewExperimental = GL_TRUE;
  CHECK_SUCCESS(glewInit() == GLEW_OK);
  glGetError();  // clear GLEW's error for it

  glfwSetKeyCallback(window, KeyCallback);
  glfwSetCursorPosCallback(window, MousePosCallback);
  glfwSetMouseButtonCallback(window, MouseButtonCallback);
  glfwSwapInterval(1);

  cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";
  cout << "OpenGL version supported:" << glGetString(GL_VERSION) << "\n";
  
  // Setup our VAOs.
  CHECK_GL_ERROR(glGenVertexArrays(kNumVaos, array_objects));

  // Generate buffer objects
  for (int i = 0; i < kNumVaos; ++i)
    CHECK_GL_ERROR(glGenBuffers(kNumVbos, &buffer_objects[i][0]));
}

bool keepLoopingOpenGL () {
  if (glfwWindowShouldClose(window))
    return false;

  glfwGetFramebufferSize(window, &window_width, &window_height);
  glViewport(0, 0, window_width, window_height);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_BLEND);
  glEnable(GL_CULL_FACE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDepthFunc(GL_LESS);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glCullFace(GL_BACK);

  // Compute our view, and projection matrices.
  if (fps_mode)
    center = eye - camera_distance * look;
  else
    eye = center + camera_distance * look;

  up = glm::vec3(0.0f, 1.0f, 0.0f);

  view_matrix = glm::lookAt(eye, center, up);
  projection_matrix = glm::perspective(static_cast<float>(kFov * (M_PI / 180.0f)),
                                       static_cast<float>(window_width) / window_height,
                                       kNear, kFar);

  return true;
}

void endLoopOpenGL () {
  glfwPollEvents();
  glfwSwapBuffers(window);
}

void cleanupOpenGL () {
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
