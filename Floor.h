#ifndef FLOOR_H
#define FLOOR_H

#include <vector>

#include "Program.h"

const float kFloorLength = 100.0f;
const float kFloorY = -1.0f;

struct FloorProgram: public Program {
  std::vector<glm::vec4> floor_vertices;
  std::vector<glm::uvec3> floor_faces;

  FloorProgram (glm::mat4* view_p, glm::mat4* proj_p) : Program(view_p, proj_p) { 
    floor_vertices.push_back(glm::vec4(-kFloorLength, kFloorY, kFloorLength, 1.0f));
    floor_vertices.push_back(glm::vec4(kFloorLength, kFloorY, kFloorLength, 1.0f));
    floor_vertices.push_back(glm::vec4(kFloorLength, kFloorY, -kFloorLength, 1.0f));
    floor_vertices.push_back(glm::vec4(-kFloorLength, kFloorY, -kFloorLength, 1.0f));

    floor_faces.push_back(glm::uvec3(0, 1, 2));
    floor_faces.push_back(glm::uvec3(2, 3, 0));
  }

  void setup();
  void draw();
};

#endif
