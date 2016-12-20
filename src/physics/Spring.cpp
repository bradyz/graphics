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

  // cout << "A: " << this->sphereA.position << endl;
  // cout << "B: " << this->sphereB.position << endl;
  // cout << "k: " << this->kHook << endl;
  // cout << "dl: " << dl << endl;
  // cout << "rl: " << this->rLength << endl;
  // cout << "dx: " << (dl - this->rLength) << endl;
  // cout << "dp: " << dp << endl;
  // cout << Fs << endl;

  vec3 dv = (this->sphereA.velocity - this->sphereB.velocity);

  // Force applied by damping.
  vec3 Fd = this->kDamp * dot(dv, dp) * dp;

  this->sphereA.applyForce(-Fs);
  this->sphereB.applyForce( Fs);

  this->sphereA.applyForce(-Fd);
  this->sphereB.applyForce( Fd);
}
