#ifndef SPRING_H
#define SPRING_H

#include "geometry/Sphere.h"

struct Spring {
  Sphere& sphereA;
  Sphere& sphereB;
  double kHook;
  double rLength;

  Spring (Sphere& sa, Sphere& sb, double k, double l) :
    sphereA(sa),
    sphereB(sb),
    kHook(k),
    rLength(l) { }

  void step ();
};

#endif
