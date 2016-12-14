#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <glm/glm.hpp>

struct Intersection {
  glm::vec3 displacement;
  bool hit;

  float timeHit;

  Intersection () : hit(false) { }
};

#endif
