#include <iostream>
#include <algorithm>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "physics/Spring.h"
#include "helpers/RandomUtils.h"

using namespace std;
using namespace glm;

void Spring::step () {
  vec3 dp = this->sphereA.position - this->sphereB.position;
  float dl = length(dp);

  // Force applied by spring
  vec3 Fs = this->kHook * (dl - this->rLength) * dp;

  this->sphereA.applyForce(-Fs);
  this->sphereB.applyForce( Fs);
}
