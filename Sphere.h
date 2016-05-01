#ifndef SPHERE_H
#define SPHERE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

struct Sphere {
  const double radius;
  glm::vec3 position;
  glm::vec3 velocity;

  Sphere (double r, const glm::vec3& pos): radius(r), position(pos) { }

  void step (const glm::vec3& force=glm::vec3(0.0, -0.001, 0.0)) { 
    velocity += force; 
    position += velocity;
  }

  glm::mat4 toWorld () const {
    glm::mat4 T = glm::translate(position);
    glm::mat4 S = glm::scale(glm::vec3(radius, radius, radius));

    return S * T;
  }
};

#endif
