#ifndef PROGRAM_H
#define PROGRAM_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct Program {
  int vaoIndex;

  GLuint programId;
  GLint projection_matrix_location = 0;
  GLint model_matrix_location = 0;
  GLint view_matrix_location = 0;
  GLint light_position_location = 0;
  
  // line segment
  GLint line_color_location = 0;

  // shadow
  GLint shadow_matrix_location = 0;

} floorProgram, lineSegmentProgram, phongProgram, shadowProgram;

#endif
