#include <vector>
#include <iostream>
#include <string>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "RandomUtils.h"
#include "OpenGLStuff.h"
#include "Shadow.h"
#include "Floor.h"

using namespace std;

string SHADOW_VERT = "./shaders/shadow.vert";
string SHADOW_GEOM = "./shaders/shadow.geom";
string SHADOW_FRAG = "./shaders/shadow.frag";

void ShadowProgram::setup () {
  this->vaoIndex = kShadowVao;

  // Setup shaders
  string vertex_shader = loadShader(SHADOW_VERT).c_str();
  GLuint vertex_shader_id = setupShader(vertex_shader.c_str(), GL_VERTEX_SHADER);

  string geometry_shader = loadShader(SHADOW_GEOM).c_str();
  GLuint geometry_shader_id = setupShader(geometry_shader.c_str(), GL_GEOMETRY_SHADER);

  string fragment_shader = loadShader(SHADOW_FRAG).c_str();
  GLuint fragment_shader_id = setupShader(fragment_shader.c_str(), GL_FRAGMENT_SHADER);

  // Let's create our floor program.
  GLuint& program_id = this->programId;
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
  GLint& shadow_matrix_location = this->shadow_matrix_location;
  CHECK_GL_ERROR(shadow_matrix_location = glGetUniformLocation(program_id, "shadow"));
  GLint& projection_matrix_location = this->projection_matrix_location;
  CHECK_GL_ERROR(projection_matrix_location = glGetUniformLocation(program_id, "projection"));
  GLint& model_matrix_location = this->model_matrix_location;
  CHECK_GL_ERROR(model_matrix_location = glGetUniformLocation(program_id, "model"));
  GLint& view_matrix_location = this->view_matrix_location;
  CHECK_GL_ERROR(view_matrix_location = glGetUniformLocation(program_id, "view"));

  glm::vec3 n(0.0f, 1.0f, 0.0f); 
  glm::vec3 b(0.0f, kFloorY + 0.08f, 0.0f);
  glm::vec3 l(LIGHT_POSITION);

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

void ShadowProgram::draw (const vector<glm::vec4>& vertices,
                          const vector<glm::uvec3>& faces,
                          const glm::mat4& model) {
  CHECK_GL_ERROR(glUseProgram(this->programId));

  CHECK_GL_ERROR(glUniformMatrix4fv(this->projection_matrix_location, 1,
                                    GL_FALSE, &this->proj[0][0]));
  CHECK_GL_ERROR(glUniformMatrix4fv(this->model_matrix_location, 1,
                                    GL_FALSE, &model[0][0]));
  CHECK_GL_ERROR(glUniformMatrix4fv(this->view_matrix_location, 
                                    1, GL_FALSE, &this->view[0][0]));
  CHECK_GL_ERROR(glUniformMatrix4fv(this->shadow_matrix_location, 
                                    1, GL_FALSE, &this->shadow_matrix[0][0]));

  CHECK_GL_ERROR(glBindVertexArray(array_objects[this->vaoIndex]));

  CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER,
                              buffer_objects[this->vaoIndex][kVertexBuffer]));
  CHECK_GL_ERROR(glBufferData(GL_ARRAY_BUFFER,
                              sizeof(float) * vertices.size() * 4,
                              &vertices[0], GL_STATIC_DRAW));

  CHECK_GL_ERROR(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0));
  CHECK_GL_ERROR(glEnableVertexAttribArray(0));

  CHECK_GL_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                              buffer_objects[this->vaoIndex][kIndexBuffer]));
  CHECK_GL_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                              sizeof(uint32_t) * faces.size() * 3,
                              &faces[0], GL_STATIC_DRAW));

  CHECK_GL_ERROR(glDrawElements(GL_TRIANGLES, faces.size() * 3,
                                GL_UNSIGNED_INT, 0));

  CHECK_GL_ERROR(glBindVertexArray(0));
}
