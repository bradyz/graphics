#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

struct Ray {
  glm::vec3 position;
  glm::vec3 direction;

  Ray(const glm::vec3 &o, const glm::vec3 &v) :
    position(o), direction(glm::normalize(v)) { }
};

#endif
