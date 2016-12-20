#ifndef SPRING_H
#define SPRING_H

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "geometry/Sphere.h"

struct Spring {
  Sphere& sphereA;
  Sphere& sphereB;
  float kHook;
  float rLength;

  Spring (Sphere& sa, Sphere& sb, double k=100000.0, double l=-1.0) :
    sphereA(sa), sphereB(sb), kHook(k) {
      rLength = (l < 0.0) ? glm::length(sa.position - sb.position) : l;
  }

  void step ();
};

#endif
