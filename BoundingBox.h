#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <algorithm>
#include <iostream>

#include <glm/glm.hpp>

const double INF = 1e9;

struct BoundingBox {
  bool isEmpty;

  glm::vec3 minVals;
  glm::vec3 maxVals;

  BoundingBox (const std::vector<glm::vec4>& points=std::vector<glm::vec4>()) :
    isEmpty(points.size() > 0), minVals(INF, INF, INF), maxVals(-INF, -INF, -INF) {
    add(points);
  }

  void add (const glm::vec3& point) {
    isEmpty = false;
    for (int i = 0; i < 3; ++i) {
      minVals[i] = fmin(minVals[i], point[i]);
      maxVals[i] = fmax(maxVals[i], point[i]);
    }
  }

  void add (const std::vector<glm::vec3>& points) {
    for (const glm::vec3& point: points)
      add(point);
  }

  void add (const std::vector<glm::vec4>& points) {
    for (const glm::vec4& point: points)
      add(glm::vec3(point));
  }

  void merge (const BoundingBox& other) {
    if (other.isEmpty)
      return;
    isEmpty = false;
    for (int i = 0; i < 3; ++i) {
      minVals[i] = fmin(minVals[i], other.minVals[i]);
      maxVals[i] = fmax(maxVals[i], other.maxVals[i]);
    }
  }

  std::vector<glm::vec4> getVertices () const {
    std::vector<glm::vec4> vertices;

    vertices.push_back(glm::vec4(minVals.x, minVals.y, minVals.z, 1.0));
    vertices.push_back(glm::vec4(maxVals.x, minVals.y, minVals.z, 1.0));
    vertices.push_back(glm::vec4(maxVals.x, minVals.y, maxVals.z, 1.0));
    vertices.push_back(glm::vec4(minVals.x, minVals.y, maxVals.z, 1.0));

    vertices.push_back(glm::vec4(minVals.x, maxVals.y, minVals.z, 1.0));
    vertices.push_back(glm::vec4(maxVals.x, maxVals.y, minVals.z, 1.0));
    vertices.push_back(glm::vec4(maxVals.x, maxVals.y, maxVals.z, 1.0));
    vertices.push_back(glm::vec4(minVals.x, maxVals.y, maxVals.z, 1.0));

    return vertices;
  }

  std::vector<glm::uvec2> getEdges () const {
    std::vector<glm::uvec2> edges;

    edges.push_back(glm::uvec2(0, 1));
    edges.push_back(glm::uvec2(1, 2));
    edges.push_back(glm::uvec2(2, 3));
    edges.push_back(glm::uvec2(3, 0));

    edges.push_back(glm::uvec2(4, 5));
    edges.push_back(glm::uvec2(5, 6));
    edges.push_back(glm::uvec2(6, 7));
    edges.push_back(glm::uvec2(7, 4));

    edges.push_back(glm::uvec2(0, 4));
    edges.push_back(glm::uvec2(1, 5));
    edges.push_back(glm::uvec2(2, 6));
    edges.push_back(glm::uvec2(3, 7));

    return edges;
  }

  friend std::ostream& operator<< (std::ostream& os, const BoundingBox& box) {
    os << "min: " << box.minVals << std::endl;
    os << "max: " << box.maxVals << std::endl;
    return os;
  }
};

#endif