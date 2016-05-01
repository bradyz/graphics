#ifndef PLANE_H
#define PLANE_H

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "BoundingBox.h"

struct Plane {
  glm::vec3 position;
  glm::vec3 normal;

  std::vector<glm::vec4> vertices;
  std::vector<glm::uvec3> faces;
  std::vector<glm::vec4> normals;

  double len;
  double wid;

  Plane (const glm::vec3& p, const glm::vec3& n, double l, double w) :
    position(p), normal(n), len(l), wid(w) {
    vertices.push_back(glm::vec4(p.x-w, p.y, p.z-l, 1.0));
    vertices.push_back(glm::vec4(p.x-w, p.y, p.z+l, 1.0));
    vertices.push_back(glm::vec4(p.x+w, p.y, p.z-l, 1.0));
    vertices.push_back(glm::vec4(p.x+w, p.y, p.z+l, 1.0));

    glm::vec3 original(0.0, 1.0, 0.0);
    glm::vec3 axis = glm::cross(normal, original);
    glm::mat4 R = glm::rotate(glm::dot(normal, original), axis);

    for (glm::vec4& vertex: vertices)
      vertex = R * vertex;

    faces.push_back(glm::uvec3(0, 1, 3));
    faces.push_back(glm::uvec3(0, 3, 2));

    normals.push_back(glm::vec4(n, 1.0));
    normals.push_back(glm::vec4(n, 1.0));
    normals.push_back(glm::vec4(n, 1.0));
    normals.push_back(glm::vec4(n, 1.0));
  }

  BoundingBox getBoundingBox () {
    return BoundingBox(vertices);
  }
};

#endif
