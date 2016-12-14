#include <vector>
#include <iostream>

#include <GL/glew.h>

#include <glm/glm.hpp>

#include "geometry/BoundingBox.h"
#include "helpers/RandomUtils.h"
#include "render/LineSegment.h"
#include "render/OpenGLStuff.h"

using namespace std;
using namespace glm;

void LineSegmentProgram::setup () {
  this->vaoIndex = kLineSegVao;

  // shaders
  string vertex_shader = loadShader(LINE_VERT).c_str();
  GLuint vertex_shader_id = setupShader(vertex_shader.c_str(), GL_VERTEX_SHADER);

  string geometry_shader = loadShader(LINE_GEOM).c_str();
  GLuint geometry_shader_id = setupShader(geometry_shader.c_str(), GL_GEOMETRY_SHADER);

  string fragment_shader = loadShader(LINE_FRAG).c_str();
  GLuint fragment_shader_id = setupShader(fragment_shader.c_str(), GL_FRAGMENT_SHADER);

  // program
  GLuint& program_id = this->programId;
  CHECK_GL_ERROR(program_id = glCreateProgram());
  CHECK_GL_ERROR(glAttachShader(program_id, vertex_shader_id));
  CHECK_GL_ERROR(glAttachShader(program_id, geometry_shader_id));
  CHECK_GL_ERROR(glAttachShader(program_id, fragment_shader_id));

  // attributes
  CHECK_GL_ERROR(glBindAttribLocation(program_id, 0, "vertex_position"));
  CHECK_GL_ERROR(glBindFragDataLocation(program_id, 0, "fragment_color"));

  glLinkProgram(program_id);
  CHECK_GL_PROGRAM_ERROR(program_id);

  // uniforms
  GLint& model_matrix_location = this->model_matrix_location;
  CHECK_GL_ERROR(model_matrix_location = glGetUniformLocation(program_id, "model"));
  GLint& view_matrix_location = this->view_matrix_location;
  CHECK_GL_ERROR(view_matrix_location = glGetUniformLocation(program_id, "view"));
  GLint& projection_matrix_location = this->projection_matrix_location;
  CHECK_GL_ERROR(projection_matrix_location = glGetUniformLocation(program_id, "projection"));

  GLint& line_color_location = this->line_color_location;
  CHECK_GL_ERROR(line_color_location = glGetUniformLocation(program_id, "line_color"));
}

void LineSegmentProgram::draw (const vector<vec4>& vertices, 
                               const vector<uvec2>& segments, 
                               const mat4& model_matrix,
                               const vec4& color) {
  CHECK_GL_ERROR(glUseProgram(this->programId));

  CHECK_GL_ERROR(glUniformMatrix4fv(this->model_matrix_location, 1, GL_FALSE,
                                    &model_matrix[0][0]));
  CHECK_GL_ERROR(glUniformMatrix4fv(this->view_matrix_location, 1, GL_FALSE,
                                    &this->view[0][0]));
  CHECK_GL_ERROR(glUniformMatrix4fv(this->projection_matrix_location, 1, GL_FALSE,
                                    &this->proj[0][0]));

  CHECK_GL_ERROR(glUniform4fv(this->line_color_location, 1, &color[0]));

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
                              sizeof(uint32_t) * segments.size() * 2,
                              &segments[0], GL_STATIC_DRAW));

  CHECK_GL_ERROR(glDrawElements(GL_LINE_STRIP, segments.size() * 2,
                                GL_UNSIGNED_INT, 0));
}

void LineSegmentProgram::drawAxis () {
  vector<vec4> x_axis_vertices;
  vector<uvec2> x_axis_segments;

  vec4 point1 = vec4(0.0f, 0.0f, 0.0f, 1.0f);
  vec4 point2 = vec4(1.0f, 0.0f, 0.0f, 1.0f);

  x_axis_vertices.push_back(point1);
  x_axis_vertices.push_back(point2);
  x_axis_segments.push_back(uvec2(1, 0));

  this->draw(x_axis_vertices, x_axis_segments, mat4(), GREEN);

  vector<vec4> z_axis_vertices;
  vector<uvec2> z_axis_segments;

  vec4 point3 = vec4(0.0f, 0.0f, 0.0f, 1.0f);
  vec4 point4 = vec4(0.0f, 0.0f, 1.0f, 1.0f);

  z_axis_vertices.push_back(point3);
  z_axis_vertices.push_back(point4);
  z_axis_segments.push_back(uvec2(1, 0));

  this->draw(z_axis_vertices, z_axis_segments, mat4(), BLUE);

  vector<vec4> y_axis_vertices;
  vector<uvec2> y_axis_segments;

  vec4 point5 = vec4(0.0f, 0.0f, 0.0f, 1.0f);
  vec4 point6 = vec4(0.0f, 1.0f, 0.0f, 1.0f);

  y_axis_vertices.push_back(point5);
  y_axis_vertices.push_back(point6);
  y_axis_segments.push_back(uvec2(1, 0));

  this->draw(y_axis_vertices, y_axis_segments, mat4(), RED);
}

void LineSegmentProgram::drawLineSegment (const vec3& u, const vec3& v,
                                          const vec4& color) {
  vector<vec4> points;
  points.push_back(vec4(u, 1.0));
  points.push_back(vec4(v, 1.0));

  vector<uvec2> edges;
  edges.push_back(uvec2(0, 1));

  draw(points, edges, mat4(), color);
}

void LineSegmentProgram::drawBoundingBox (const BoundingBox& box,
                                          const vec4& color=RED) {
  vector<vec4> vertices = box.getVertices();
  vector<uvec2> edges = box.getEdges();
  for (uvec2 edge: edges) {
    vec3 u = vec3(vertices[edge[0]]);
    vec3 v = vec3(vertices[edge[1]]);
    drawLineSegment(u, v, color);
  }
}
