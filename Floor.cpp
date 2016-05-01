#include <iostream>
#include <string>

#include "Floor.h"
#include "OpenGLStuff.h"
#include "RandomUtils.h"

using namespace std;

string FLOOR_VERT = "./shaders/basic.vert";
string FLOOR_GEOM = "./shaders/floor.geom";
string FLOOR_FRAG = "./shaders/floor.frag";

void FloorProgram::setup () {
  this->vaoIndex = kFloorVao;

  // Setup shaders
  string vertex_shader = loadShader(FLOOR_VERT).c_str();
  GLuint vertex_shader_id = setupShader(vertex_shader.c_str(), GL_VERTEX_SHADER);

  string geometry_shader = loadShader(FLOOR_GEOM).c_str();
  GLuint geometry_shader_id = setupShader(geometry_shader.c_str(), GL_GEOMETRY_SHADER);

  string fragment_shader = loadShader(FLOOR_FRAG).c_str();
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
  GLint& projection_matrix_location = this->projection_matrix_location;
  CHECK_GL_ERROR(projection_matrix_location = glGetUniformLocation(program_id, "projection"));

  GLint& view_matrix_location = this->view_matrix_location;
  CHECK_GL_ERROR(view_matrix_location = glGetUniformLocation(program_id, "view"));

  GLint& light_position_location = this->light_position_location;
  CHECK_GL_ERROR(light_position_location = glGetUniformLocation(program_id, "light_position"));

  CHECK_GL_ERROR(glBindVertexArray(array_objects[this->vaoIndex]));

  CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER,
                              buffer_objects[this->vaoIndex][kVertexBuffer]));
  CHECK_GL_ERROR(glBufferData(GL_ARRAY_BUFFER,
                              sizeof(float) * this->floor_vertices.size() * 4,
                              &this->floor_vertices[0], GL_STATIC_DRAW));

  CHECK_GL_ERROR(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0));
  CHECK_GL_ERROR(glEnableVertexAttribArray(0));

  CHECK_GL_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                              buffer_objects[this->vaoIndex][kIndexBuffer]));
  CHECK_GL_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                              sizeof(uint32_t) * this->floor_faces.size() * 3,
                              &this->floor_faces[0], GL_STATIC_DRAW));

  CHECK_GL_ERROR(glBindVertexArray(0));
}

void FloorProgram::draw () {
  CHECK_GL_ERROR(glUseProgram(this->programId));

  CHECK_GL_ERROR(glUniform4fv(this->light_position_location,
                              1, &LIGHT_POSITION[0]));
  CHECK_GL_ERROR(glUniformMatrix4fv(this->projection_matrix_location, 1,
                                    GL_FALSE, &this->proj[0][0]));
  CHECK_GL_ERROR(glUniformMatrix4fv(this->view_matrix_location, 1, GL_FALSE,
                                    &this->view[0][0]));

  CHECK_GL_ERROR(glBindVertexArray(array_objects[this->vaoIndex]));
  CHECK_GL_ERROR(glDrawElements(GL_TRIANGLES, this->floor_faces.size() * 3,
                                GL_UNSIGNED_INT, 0));

  CHECK_GL_ERROR(glBindVertexArray(0));
}

