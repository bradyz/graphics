#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <iomanip> 

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>

#include "RandomUtils.h"
#include "Sphere.h"
#include "Program.h"

using namespace std;

const string window_title = "Collision Detection";

const int WIDTH = 800;
const int HEIGHT = 600;
int window_width = WIDTH;
int window_height = HEIGHT;

const float INF = 1e9;
const float kNear = 0.0001f;
const float kFar = 1000.0f;
const float kFov = 45.0f;
float aspect = static_cast<float>(window_width) / window_height;

// Floor info.
const float eps = 0.5 * (0.025 + 0.0175);
const float kFloorXMin = -100.0f;
const float kFloorXMax = 100.0f;
const float kFloorZMin = -100.0f;
const float kFloorZMax = 100.0f;
const float kFloorY = -0.75617 - eps;

const float kAxisLength = 0.5f;

glm::vec4 light_position = glm::vec4(3.0f, 3.0f, 3.0f, 1.0f);

vector<glm::vec4> floor_vertices;
vector<glm::uvec3> floor_faces;

vector<glm::vec4> sphere_vertices;
vector<glm::uvec3> sphere_faces;
vector<glm::vec4> sphere_normals;

glm::mat4 shadow_matrix;

vector<Sphere> objects;

enum {
  kMouseModeCamera,
  kNumMouseModes
};

int current_mouse_mode = 0;

// We have these VBOs available for each VAO.
enum {
  kVertexBuffer,
  kIndexBuffer,
  kVertexNormalBuffer,
  kNumVbos
};

// These are our VAOs.
enum {
  kFloorVao,
  kLineSegVao,
  kPhongVao,
  kShadowVao,
  kNumVaos
};

GLuint array_objects[kNumVaos];             // This will store the VAO descriptors.
GLuint buffer_objects[kNumVaos][kNumVbos];  // These will store VBO descriptors.

float last_x = 0.0f, last_y = 0.0f, current_x = 0.0f, current_y = 0.0f;
bool drag_state = false;
bool button_press = false;
int current_button = -1;
float camera_distance = 2.0;
const float pan_speed = 0.1f;
const float roll_speed = 0.1f;
const float rotation_speed = 0.05f;
const float zoom_speed = 0.1f;
glm::vec3 eye = glm::vec3(0.0f, 0.1f, camera_distance - 2.0f);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 look = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 tangent = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 center = eye + camera_distance * look;
glm::mat3 orientation = glm::mat3(tangent, up, look);
bool fps_mode = false;

glm::mat4 view_matrix = glm::lookAt(eye, center, up);
glm::mat4 projection_matrix = glm::perspective((float)(kFov * (M_PI / 180.0f)),
                                               aspect, kNear, kFar);
glm::mat4 model_matrix = glm::mat4(1.0f);
glm::mat4 floor_model_matrix = glm::mat4(1.0f);

const char* vertex_shader =
    "#version 330 core\n"
    "uniform vec4 light_position;"
    "in vec4 vertex_position;"
    "out vec4 vs_light_direction;"
    "void main() {"
    " gl_Position = vertex_position;"
    " vs_light_direction = light_position - gl_Position;"
    "}";

const char* floor_geometry_shader =
    "#version 330 core\n"
    "layout (triangles) in;"
    "layout (triangle_strip, max_vertices = 3) out;"
    "uniform mat4 projection;"
    "uniform mat4 view;"
    "in vec4 vs_light_direction[];"
    "out vec4 light_direction;"
    "out vec4 world_position;"
    "void main() {"
    " for (int n = 0; n < gl_in.length(); n++) {"
    "   light_direction = normalize(vs_light_direction[n]);"
    "   gl_Position = projection * view * gl_in[n].gl_Position;"
    "   world_position = gl_in[n].gl_Position;"
    "   EmitVertex();"
    " }"
    " EndPrimitive();"
    "}";

const char* floor_fragment_shader =
    "#version 330 core\n"
    "in vec4 face_normal;"
    "in vec4 light_direction;"
    "in vec4 world_position;"
    "out vec4 fragment_color;"
    "void main() {"
    " vec4 n = vec4(0.0f, 1.0f, 0.0f, 0.0f);"
    " vec4 pos = world_position;"
    " float check_width = 0.25;"
    " float i = floor(pos.x / check_width);"
    " float j  = floor(pos.z / check_width);"
    " vec3 color = mod(i + j, 2) * vec3(1.0, 1.0, 1.0);"
    " float dot_nl = dot(normalize(light_direction), normalize(n));"
    " dot_nl = clamp(dot_nl, 0.0, 1.0);"
    " color = clamp(dot_nl * color, 0.0, 1.0);"
    " fragment_color = vec4(color, 1.0);"
    "}";

const char* line_segment_geometry_shader =
    "#version 330 core\n"
    "layout (lines) in;"
    "layout (line_strip, max_vertices = 2) out;"
    "uniform mat4 projection;"
    "uniform mat4 model;"
    "uniform mat4 view;"
    "void main() {"
    " gl_Position = projection * view * model * gl_in[0].gl_Position;"
    " EmitVertex();"
    " gl_Position = projection * view * model * gl_in[1].gl_Position;"
    " EmitVertex();"
    " EndPrimitive();"
    "}";

const char* line_segment_fragment_shader =
    "#version 330 core\n"
    "uniform vec4 line_color;"
    "out vec4 fragment_color;"
    "void main() {"
    " fragment_color = line_color;"
    "}";

const char* phong_vertex_shader =
    "#version 330 core\n"
    "uniform vec4 light_position;"
    "in vec4 vertex_position;"
    "in vec4 vertex_normal;"
    "out vec4 vs_light_direction;"
    "out vec4 vs_vertex_normal;"
    "void main() {"
    " gl_Position = vertex_position;"
    " vs_light_direction = light_position - gl_Position;"
    " vs_vertex_normal = vertex_normal;"
    "}";

const char* phong_geometry_shader =
    "#version 330 core\n"
    "layout (triangles) in;"
    "layout (triangle_strip, max_vertices = 3) out;"
    "uniform mat4 projection;"
    "uniform mat4 model;"
    "uniform mat4 view;"
    "in vec4 vs_light_direction[];"
    "in vec4 vs_vertex_normal[];"
    "out vec4 face_normal;"
    "out vec4 light_direction;"
    "void main() {"
    " for (int i = 0; i < gl_in.length(); i++) {"
    "   face_normal = model * vs_vertex_normal[i];"
    "   light_direction = normalize(vs_light_direction[i]);"
    "   gl_Position = projection * view * model * gl_in[i].gl_Position;"
    "   EmitVertex();"
    " }"
    " EndPrimitive();"
    "}";

const char* phong_fragment_shader =
    "#version 330 core\n"
    "in vec4 face_normal;"
    "in vec4 light_direction;"
    "out vec4 fragment_color;"
    "void main() {"
    " float dot_nl = dot(normalize(light_direction), normalize(face_normal));"
    " vec3 color = vec3(0.0f, 1.0f, 0.0f);"
    " fragment_color = vec4(color * clamp(dot_nl, 0.0f, 1.0f), 0.5f);"
    "}";

const char* shadow_vertex_shader =
    "#version 330 core\n"
    "in vec4 vertex_position;"
    "void main() {"
    " gl_Position = vertex_position;"
    "}";

const char* shadow_geometry_shader =
    "#version 330 core\n"
    "layout (triangles) in;"
    "layout (triangle_strip, max_vertices = 3) out;"
    "uniform mat4 projection;"
    "uniform mat4 model;"
    "uniform mat4 view;"
    "uniform mat4 shadow;"
    "void main() {"
    " for (int i = 0; i < gl_in.length(); i++) {"
    "   vec4 pos = shadow * model * gl_in[i].gl_Position;"
    "   pos /= pos.w;"
    "   gl_Position = projection * view * pos;"
    "   EmitVertex();"
    " }"
    " EndPrimitive();"
    "}";

const char* shadow_fragment_shader =
    "#version 330 core\n"
    "out vec4 fragment_color;"
    "void main() {"
    " fragment_color = vec4(0.0f, 0.0f, 0.0f, 0.8f);"
    "}";

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
      string log(length, 0);                                           \
      glGetShaderInfoLog(id, length, nullptr, &log[0]);                     \
      cerr << "Line :" << __LINE__ << " OpenGL Shader Error: Log = \n" \
                << &log[0];                                                 \
      glfwTerminate();                                                      \
      exit(EXIT_FAILURE);                                                   \
    }                                                                       \
  }

#define CHECK_GL_PROGRAM_ERROR(id)                                           \
  {                                                                          \
    GLint status = 0;                                                        \
    GLint length = 0;                                                        \
    glGetProgramiv(id, GL_LINK_STATUS, &status);                             \
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);                         \
    if (!status) {                                                           \
      string log(length, 0);                                            \
      glGetProgramInfoLog(id, length, nullptr, &log[0]);                     \
      cerr << "Line :" << __LINE__ << " OpenGL Program Error: Log = \n" \
                << &log[0];                                                  \
      glfwTerminate();                                                       \
      exit(EXIT_FAILURE);                                                    \
    }                                                                        \
  }

#define CHECK_GL_ERROR(statement)                                             \
  {                                                                           \
    { statement; }                                                            \
    GLenum error = GL_NO_ERROR;                                               \
    if ((error = glGetError()) != GL_NO_ERROR) {                              \
      cerr << "Line :" << __LINE__ << " OpenGL Error: code  = " << error \
                << " description =  " << OpenGlErrorToString(error);          \
      glfwTerminate();                                                        \
      exit(EXIT_FAILURE);                                                     \
    }                                                                         \
  }

template <typename T>
ostream& operator<<(ostream& os, const vector<T>& v) {
  size_t count = min(v.size(), static_cast<size_t>(10));
  for (size_t i = 0; i < count; ++i) os << i << " " << v[i] << "\n";
  os << "size = " << v.size() << "\n";
  return os;
}

namespace glm {
ostream& operator<<(ostream& os, const glm::vec2& v) {
  os << glm::to_string(v);
  return os;
}

ostream& operator<<(ostream& os, const glm::vec3& v) {
  os << glm::to_string(v);
  return os;
}

ostream& operator<<(ostream& os, const glm::vec4& v) {
  os << glm::to_string(v);
  return os;
}

ostream& operator<<(ostream& os, const glm::mat4& v) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j)
      cout << setprecision(3) << v[j][i] << "\t";
    cout << endl;
  }
  return os;
}

ostream& operator<<(ostream& os, const glm::mat3& v) {
  os << glm::to_string(v);
  return os;
}
}  // namespace glm

void ErrorCallback (int error, const char* description) {
  cerr << "GLFW Error: " << description << "\n";
}

void KeyCallback (GLFWwindow* window, int key, int scancode, int action,
                 int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
  else if (key == GLFW_KEY_W && action != GLFW_RELEASE) {
    if (fps_mode)
      eye -= zoom_speed * look;
    else
      camera_distance = fmax(0.1f, camera_distance - zoom_speed);
  } 
  else if (key == GLFW_KEY_S && action != GLFW_RELEASE) {
    if (fps_mode)
      eye += zoom_speed * look;
    else
      camera_distance += zoom_speed;
  } 
  else if (key == GLFW_KEY_A && action != GLFW_RELEASE) {
    if (fps_mode)
      eye -= pan_speed * tangent;
    else
      center -= pan_speed * tangent;
  } 
  else if (key == GLFW_KEY_D && action != GLFW_RELEASE) {
    if (fps_mode)
      eye += pan_speed * tangent;
    else
      center += pan_speed * tangent;
  } 
  else if (key == GLFW_KEY_C && action != GLFW_RELEASE) {
    fps_mode = !fps_mode;
  } 
  else if (key == GLFW_KEY_M && action != GLFW_RELEASE) {
    current_mouse_mode = (current_mouse_mode + 1) % kNumMouseModes;
  } 
}

GLuint setupShader (const char* shaderName, GLenum shaderType) {
  GLuint shaderId = 0;
  CHECK_GL_ERROR(shaderId = glCreateShader(shaderType));
  CHECK_GL_ERROR(glShaderSource(shaderId, 1,
                                &shaderName, nullptr));
  glCompileShader(shaderId);
  CHECK_GL_SHADER_ERROR(shaderId);
  return shaderId;
}

void setupFloorProgram () {
  floorProgram.vaoIndex = kFloorVao;

  // Setup shaders
  GLuint vertex_shader_id = setupShader(vertex_shader, GL_VERTEX_SHADER);
  GLuint geometry_shader_id = setupShader(floor_geometry_shader, GL_GEOMETRY_SHADER);
  GLuint fragment_shader_id = setupShader(floor_fragment_shader, GL_FRAGMENT_SHADER);

  // Let's create our floor program.
  GLuint& program_id = floorProgram.programId;
  CHECK_GL_ERROR(program_id = glCreateProgram());
  CHECK_GL_ERROR(glAttachShader(program_id, vertex_shader_id));
  CHECK_GL_ERROR(glAttachShader(program_id, geometry_shader_id));
  CHECK_GL_ERROR(glAttachShader(program_id, fragment_shader_id));

  // Bind attributes.
  CHECK_GL_ERROR(glBindAttribLocation(program_id, 0, "vertex_position"));
  CHECK_GL_ERROR(glBindFragDataLocation(program_id, 0, "fragment_color"));

  glLinkProgram(program_id);
  CHECK_GL_PROGRAM_ERROR(program_id);

  // Get the uniform locations.
  GLint& projection_matrix_location = floorProgram.projection_matrix_location;
  CHECK_GL_ERROR(projection_matrix_location = glGetUniformLocation(program_id, "projection"));
  GLint& view_matrix_location = floorProgram.view_matrix_location;
  CHECK_GL_ERROR(view_matrix_location = glGetUniformLocation(program_id, "view"));
  GLint& light_position_location = floorProgram.light_position_location;
  CHECK_GL_ERROR(light_position_location = glGetUniformLocation(program_id, "light_position"));

  floor_vertices.push_back(glm::vec4(kFloorXMin, kFloorY, kFloorZMax, 1.0f));
  floor_vertices.push_back(glm::vec4(kFloorXMax, kFloorY, kFloorZMax, 1.0f));
  floor_vertices.push_back(glm::vec4(kFloorXMax, kFloorY, kFloorZMin, 1.0f));
  floor_vertices.push_back(glm::vec4(kFloorXMin, kFloorY, kFloorZMin, 1.0f));
  floor_faces.push_back(glm::uvec3(0, 1, 2));
  floor_faces.push_back(glm::uvec3(2, 3, 0));

  CHECK_GL_ERROR(glBindVertexArray(array_objects[floorProgram.vaoIndex]));

  CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER,
                              buffer_objects[floorProgram.vaoIndex][kVertexBuffer]));
  CHECK_GL_ERROR(glBufferData(GL_ARRAY_BUFFER,
                              sizeof(float) * floor_vertices.size() * 4,
                              &floor_vertices[0], GL_STATIC_DRAW));

  CHECK_GL_ERROR(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0));
  CHECK_GL_ERROR(glEnableVertexAttribArray(0));

  CHECK_GL_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                              buffer_objects[floorProgram.vaoIndex][kIndexBuffer]));
  CHECK_GL_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                              sizeof(uint32_t) * floor_faces.size() * 3,
                              &floor_faces[0], GL_STATIC_DRAW));

  CHECK_GL_ERROR(glBindVertexArray(0));
}

void setupLineSegmentProgram () {
  lineSegmentProgram.vaoIndex = kLineSegVao;

  // Setup shaders
  GLuint vertex_shader_id = setupShader(vertex_shader, GL_VERTEX_SHADER);
  GLuint geometry_shader_id = setupShader(line_segment_geometry_shader, GL_GEOMETRY_SHADER);
  GLuint fragment_shader_id = setupShader(line_segment_fragment_shader, GL_FRAGMENT_SHADER);

  // Let's create our floor program.
  GLuint& program_id = lineSegmentProgram.programId;
  CHECK_GL_ERROR(program_id = glCreateProgram());
  CHECK_GL_ERROR(glAttachShader(program_id, vertex_shader_id));
  CHECK_GL_ERROR(glAttachShader(program_id, geometry_shader_id));
  CHECK_GL_ERROR(glAttachShader(program_id, fragment_shader_id));

  // Bind attributes.
  CHECK_GL_ERROR(glBindAttribLocation(program_id, 0, "vertex_position"));
  CHECK_GL_ERROR(glBindFragDataLocation(program_id, 0, "fragment_color"));

  glLinkProgram(program_id);
  CHECK_GL_PROGRAM_ERROR(program_id);

  // Get the uniform locations.
  GLint& projection_matrix_location = lineSegmentProgram.projection_matrix_location;
  CHECK_GL_ERROR(projection_matrix_location = glGetUniformLocation(program_id, "projection"));
  GLint& model_matrix_location = lineSegmentProgram.model_matrix_location;
  CHECK_GL_ERROR(model_matrix_location = glGetUniformLocation(program_id, "model"));
  GLint& view_matrix_location = lineSegmentProgram.view_matrix_location;
  CHECK_GL_ERROR(view_matrix_location = glGetUniformLocation(program_id, "view"));
  GLint& line_color_location = lineSegmentProgram.line_color_location;
  CHECK_GL_ERROR(line_color_location = glGetUniformLocation(program_id, "line_color"));
}

void setupPhongProgram () {
  phongProgram.vaoIndex = kPhongVao;

  // Setup shaders
  GLuint vertex_shader_id = setupShader(phong_vertex_shader, GL_VERTEX_SHADER);
  GLuint geometry_shader_id = setupShader(phong_geometry_shader, GL_GEOMETRY_SHADER);
  GLuint fragment_shader_id = setupShader(phong_fragment_shader, GL_FRAGMENT_SHADER);

  // Let's create our floor program.
  GLuint& program_id = phongProgram.programId;
  CHECK_GL_ERROR(program_id = glCreateProgram());
  CHECK_GL_ERROR(glAttachShader(program_id, vertex_shader_id));
  CHECK_GL_ERROR(glAttachShader(program_id, geometry_shader_id));
  CHECK_GL_ERROR(glAttachShader(program_id, fragment_shader_id));

  // Bind attributes.
  CHECK_GL_ERROR(glBindAttribLocation(program_id, 0, "vertex_position"));
  CHECK_GL_ERROR(glBindAttribLocation(program_id, 1, "vertex_normal"));
  CHECK_GL_ERROR(glBindFragDataLocation(program_id, 0, "fragment_color"));

  glLinkProgram(program_id);
  CHECK_GL_PROGRAM_ERROR(program_id);

  // Get the uniform locations.
  GLint& projection_matrix_location = phongProgram.projection_matrix_location;
  CHECK_GL_ERROR(projection_matrix_location = glGetUniformLocation(program_id, "projection"));
  GLint& model_matrix_location = phongProgram.model_matrix_location;
  CHECK_GL_ERROR(model_matrix_location = glGetUniformLocation(program_id, "model"));
  GLint& view_matrix_location = phongProgram.view_matrix_location;
  CHECK_GL_ERROR(view_matrix_location = glGetUniformLocation(program_id, "view"));
  GLint& light_position_location = phongProgram.light_position_location;
  CHECK_GL_ERROR(light_position_location = glGetUniformLocation(program_id, "light_position"));
}

void setupShadowProgram () {
  shadowProgram.vaoIndex = kShadowVao;

  // Setup shaders
  GLuint vertex_shader_id = setupShader(shadow_vertex_shader, GL_VERTEX_SHADER);
  GLuint geometry_shader_id = setupShader(shadow_geometry_shader, GL_GEOMETRY_SHADER);
  GLuint fragment_shader_id = setupShader(shadow_fragment_shader, GL_FRAGMENT_SHADER);

  // Let's create our floor program.
  GLuint& program_id = shadowProgram.programId;
  CHECK_GL_ERROR(program_id = glCreateProgram());
  CHECK_GL_ERROR(glAttachShader(program_id, vertex_shader_id));
  CHECK_GL_ERROR(glAttachShader(program_id, geometry_shader_id));
  CHECK_GL_ERROR(glAttachShader(program_id, fragment_shader_id));

  // Bind attributes.
  CHECK_GL_ERROR(glBindAttribLocation(program_id, 0, "vertex_position"));
  CHECK_GL_ERROR(glBindFragDataLocation(program_id, 0, "fragment_color"));

  glLinkProgram(program_id);
  CHECK_GL_PROGRAM_ERROR(program_id);

  // Get the uniform locations.
  GLint& shadow_matrix_location = shadowProgram.shadow_matrix_location;
  CHECK_GL_ERROR(shadow_matrix_location = glGetUniformLocation(program_id, "shadow"));
  GLint& projection_matrix_location = shadowProgram.projection_matrix_location;
  CHECK_GL_ERROR(projection_matrix_location = glGetUniformLocation(program_id, "projection"));
  GLint& model_matrix_location = shadowProgram.model_matrix_location;
  CHECK_GL_ERROR(model_matrix_location = glGetUniformLocation(program_id, "model"));
  GLint& view_matrix_location = shadowProgram.view_matrix_location;
  CHECK_GL_ERROR(view_matrix_location = glGetUniformLocation(program_id, "view"));

  glm::vec3 n(0.0f, 1.0f, 0.0f); 
  glm::vec3 b(0.0f, kFloorY+4*eps, 0.0f);
  glm::vec3 l(light_position);

  glm::mat4 M = glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, n.x),
                          glm::vec4(0.0f, 1.0f, 0.0f, n.y),
                          glm::vec4(0.0f, 0.0f, 1.0f, n.z),
                          glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

  float c = glm::dot(b - l, n);

  shadow_matrix = glm::translate(-l);
  shadow_matrix = M * shadow_matrix;
  shadow_matrix = glm::scale(glm::vec3(c, c, c)) * shadow_matrix;
  shadow_matrix = glm::translate(l) * shadow_matrix;
}

void drawFloor () {
  CHECK_GL_ERROR(glUseProgram(floorProgram.programId));


  CHECK_GL_ERROR(glUniform4fv(floorProgram.light_position_location,
                              1, &light_position[0]));
  CHECK_GL_ERROR(glUniformMatrix4fv(floorProgram.projection_matrix_location, 1,
                                    GL_FALSE, &projection_matrix[0][0]));
  CHECK_GL_ERROR(glUniformMatrix4fv(floorProgram.view_matrix_location, 1, GL_FALSE,
                                    &view_matrix[0][0]));

  CHECK_GL_ERROR(glBindVertexArray(array_objects[floorProgram.vaoIndex]));
  CHECK_GL_ERROR(glDrawElements(GL_TRIANGLES, floor_faces.size() * 3,
                                GL_UNSIGNED_INT, 0));
  CHECK_GL_ERROR(glBindVertexArray(0));
}

void drawLineSegment (const vector<glm::vec4>& vertices, 
                      const vector<glm::uvec2>& segments, 
                      const glm::mat4& model_matrix,
                      const glm::vec4& color) {
  CHECK_GL_ERROR(glUseProgram(lineSegmentProgram.programId));

  CHECK_GL_ERROR(glUniformMatrix4fv(lineSegmentProgram.projection_matrix_location, 1,
                                    GL_FALSE, &projection_matrix[0][0]));
  CHECK_GL_ERROR(glUniformMatrix4fv(lineSegmentProgram.view_matrix_location, 
                                    1, GL_FALSE, &view_matrix[0][0]));
  CHECK_GL_ERROR(glUniformMatrix4fv(lineSegmentProgram.model_matrix_location,
                                    1, GL_FALSE, &model_matrix[0][0]));
  CHECK_GL_ERROR(glUniform4fv(lineSegmentProgram.line_color_location,
                              1, &color[0]));

  CHECK_GL_ERROR(glBindVertexArray(array_objects[lineSegmentProgram.vaoIndex]));

  CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER,
                              buffer_objects[lineSegmentProgram.vaoIndex][kVertexBuffer]));
  CHECK_GL_ERROR(glBufferData(GL_ARRAY_BUFFER,
                              sizeof(float) * vertices.size() * 4,
                              &vertices[0], GL_STATIC_DRAW));

  CHECK_GL_ERROR(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0));
  CHECK_GL_ERROR(glEnableVertexAttribArray(0));

  CHECK_GL_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                              buffer_objects[lineSegmentProgram.vaoIndex][kIndexBuffer]));
  CHECK_GL_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                              sizeof(uint32_t) * segments.size() * 2,
                              &segments[0], GL_STATIC_DRAW));

  CHECK_GL_ERROR(glDrawElements(GL_LINE_STRIP, segments.size() * 2,
                                GL_UNSIGNED_INT, 0));
}

void drawAxis () {
  vector<glm::vec4> x_axis_vertices;
  vector<glm::uvec2> x_axis_segments;

  glm::vec4 point1 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  glm::vec4 point2 = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

  x_axis_vertices.push_back(point1);
  x_axis_vertices.push_back(point2);
  x_axis_segments.push_back(glm::uvec2(1, 0));

  drawLineSegment(x_axis_vertices, x_axis_segments,
                  glm::mat4(), GREEN);

  vector<glm::vec4> z_axis_vertices;
  vector<glm::uvec2> z_axis_segments;

  glm::vec4 point3 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  glm::vec4 point4 = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

  z_axis_vertices.push_back(point3);
  z_axis_vertices.push_back(point4);
  z_axis_segments.push_back(glm::uvec2(1, 0));

  drawLineSegment(z_axis_vertices, z_axis_segments,
                  glm::mat4(), BLUE);

  vector<glm::vec4> y_axis_vertices;
  vector<glm::uvec2> y_axis_segments;

  glm::vec4 point5 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  glm::vec4 point6 = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

  y_axis_vertices.push_back(point5);
  y_axis_vertices.push_back(point6);
  y_axis_segments.push_back(glm::uvec2(1, 0));

  drawLineSegment(y_axis_vertices, y_axis_segments,
                  glm::mat4(), RED);
}

void drawMouse (const glm::vec3& mouse) {
  glm::vec4 mouse4 = glm::vec4(mouse, 1.0f);
  glm::vec4 start = glm::vec4(eye.x, eye.y-0.001f, eye.z, 1.0f);
  glm::vec4 end = (mouse4 - start) * 10.0f + mouse4;

  vector<glm::vec4> vertices;
  vertices.push_back(start);
  vertices.push_back(end);

  vector<glm::uvec2> segments;
  segments.push_back(glm::uvec2(0, 1));

  drawLineSegment(vertices, segments, glm::mat4(), WHITE);
}

glm::vec3 mouseWorld (GLFWwindow* window) {
  glm::vec4 viewport = glm::vec4(0.0f, 0.0f, WIDTH, HEIGHT);

  double x, y;
  float z;

  glfwGetCursorPos(window, &x, &y);
  glReadPixels(x, viewport[3]-y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);

  glm::vec3 mouseNDC = glm::vec3(x, viewport[3]-y, z);

  return glm::unProject(mouseNDC, view_matrix, projection_matrix, viewport);
}

void drawPhongGreen (const vector<glm::vec4>& vertices,
                     const vector<glm::uvec3>& faces,
                     const vector<glm::vec4>& normals,
                     const glm::mat4& model) {
  CHECK_GL_ERROR(glUseProgram(phongProgram.programId));

  CHECK_GL_ERROR(glUniformMatrix4fv(phongProgram.projection_matrix_location, 1,
                                    GL_FALSE, &projection_matrix[0][0]));
  CHECK_GL_ERROR(glUniformMatrix4fv(phongProgram.model_matrix_location, 1, GL_FALSE,
                                    &model[0][0]));
  CHECK_GL_ERROR(glUniformMatrix4fv(phongProgram.view_matrix_location, 1, GL_FALSE,
                                    &view_matrix[0][0]));
  CHECK_GL_ERROR(glUniform4fv(phongProgram.light_position_location,
                              1, &light_position[0]));

  CHECK_GL_ERROR(glBindVertexArray(array_objects[phongProgram.vaoIndex]));

  CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER,
                              buffer_objects[phongProgram.vaoIndex][kVertexBuffer]));
  CHECK_GL_ERROR(glBufferData(GL_ARRAY_BUFFER,
                              sizeof(float) * vertices.size() * 4,
                              &vertices[0], GL_STATIC_DRAW));

  CHECK_GL_ERROR(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0));
  CHECK_GL_ERROR(glEnableVertexAttribArray(0));

  CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, 
                              buffer_objects[phongProgram.vaoIndex][kVertexNormalBuffer]));
  CHECK_GL_ERROR(glBufferData(GL_ARRAY_BUFFER,
                              sizeof(float) * normals.size() * 4,
                              &normals[0], GL_STATIC_DRAW));

  CHECK_GL_ERROR(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0));
  CHECK_GL_ERROR(glEnableVertexAttribArray(1));

  CHECK_GL_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                              buffer_objects[phongProgram.vaoIndex][kIndexBuffer]));
  CHECK_GL_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                              sizeof(uint32_t) * faces.size() * 3,
                              &faces[0], GL_STATIC_DRAW));

  CHECK_GL_ERROR(glDrawElements(GL_TRIANGLES, faces.size() * 3,
                                GL_UNSIGNED_INT, 0));
}

void drawShadow (const vector<glm::vec4>& vertices,
                 const vector<glm::uvec3>& faces,
                 const glm::mat4& model) {
  CHECK_GL_ERROR(glUseProgram(shadowProgram.programId));

  CHECK_GL_ERROR(glUniformMatrix4fv(shadowProgram.projection_matrix_location, 1,
                                    GL_FALSE, &projection_matrix[0][0]));
  CHECK_GL_ERROR(glUniformMatrix4fv(shadowProgram.model_matrix_location, 1,
                                    GL_FALSE, &model[0][0]));
  CHECK_GL_ERROR(glUniformMatrix4fv(shadowProgram.view_matrix_location, 
                                    1, GL_FALSE, &view_matrix[0][0]));
  CHECK_GL_ERROR(glUniformMatrix4fv(shadowProgram.shadow_matrix_location, 
                                    1, GL_FALSE, &shadow_matrix[0][0]));

  CHECK_GL_ERROR(glBindVertexArray(array_objects[shadowProgram.vaoIndex]));

  CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER,
                              buffer_objects[shadowProgram.vaoIndex][kVertexBuffer]));
  CHECK_GL_ERROR(glBufferData(GL_ARRAY_BUFFER,
                              sizeof(float) * vertices.size() * 4,
                              &vertices[0], GL_STATIC_DRAW));

  CHECK_GL_ERROR(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0));
  CHECK_GL_ERROR(glEnableVertexAttribArray(0));

  CHECK_GL_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                              buffer_objects[shadowProgram.vaoIndex][kIndexBuffer]));
  CHECK_GL_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                              sizeof(uint32_t) * faces.size() * 3,
                              &faces[0], GL_STATIC_DRAW));

  CHECK_GL_ERROR(glDrawElements(GL_TRIANGLES, faces.size() * 3,
                                GL_UNSIGNED_INT, 0));

  CHECK_GL_ERROR(glBindVertexArray(0));
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

void updatePosition () {
  
}

int main(int argc, char* argv[]) {
  if (!glfwInit()) exit(EXIT_FAILURE);
  glfwSetErrorCallback(ErrorCallback);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);
  GLFWwindow* window = glfwCreateWindow(window_width, window_height,
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
  const GLubyte* renderer = glGetString(GL_RENDERER);  // get renderer string
  const GLubyte* version = glGetString(GL_VERSION);    // version as a string
  cout << "Renderer: " << renderer << "\n";
  cout << "OpenGL version supported:" << version << "\n";

  LoadOBJ("./obj/sphere.obj", sphere_vertices, sphere_faces);
  sphere_normals = getVertexNormals(sphere_vertices, sphere_faces);

  vector<glm::vec4> armadillo_vertices;
  vector<glm::uvec3> armadillo_faces;
  vector<glm::vec4> armadillo_normals;

  LoadOBJWithNormals("./obj/armadillo.obj", armadillo_vertices, armadillo_faces, armadillo_normals);

  // Setup our VAOs.
  CHECK_GL_ERROR(glGenVertexArrays(kNumVaos, array_objects));

  // Generate buffer objects
  for (int i = 0; i < kNumVaos; ++i)
    CHECK_GL_ERROR(glGenBuffers(kNumVbos, &buffer_objects[i][0]));

  setupPhongProgram();
  setupShadowProgram();
  setupLineSegmentProgram();
  setupFloorProgram();

  objects.push_back(Sphere(5, glm::vec3(0.0, 1.0, 0.0)));
  objects.push_back(Sphere(3, glm::vec3(0.5, 1.0, 0.0)));

  while (glfwWindowShouldClose(window) == false) {
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

    aspect = static_cast<float>(window_width) / window_height;
    projection_matrix = glm::perspective((float)(kFov * (M_PI / 180.0f)),
                                         aspect, kNear, kFar);
    model_matrix = glm::mat4(1.0f);

    float dist = 1.5f;

    drawPhongGreen(armadillo_vertices, armadillo_faces, armadillo_normals, glm::mat4());

    drawAxis();
    drawFloor();
    drawMouse(mouseWorld(window));

    // Poll and swap.
    glfwPollEvents();
    glfwSwapBuffers(window);
  }
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
