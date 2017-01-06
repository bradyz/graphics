#ifndef OCTREE_H
#define OCTREE_H

#include <vector>

#include "physics/RigidBody.h"
#include "geometry/BoundingBox.h"
#include "geometry/Sphere.h"
#include "geometry/Geometry.h"

struct OctTreeNode {
  BoundingBox box;
  OctTreeNode* cells[8];
  std::vector<RigidBody*> objects;

  OctTreeNode (const std::vector<RigidBody*>& newObjects, int level=0);

  // OctTreeNode (const std::vector<Geometry*>& newObjects, int level=0);

  void getAllBoxes (std::vector<BoundingBox>& allBoxes) const;

  ~OctTreeNode () {
    for (int i = 0; i < 8; ++i) {
      if (cells[i] != NULL)
        delete cells[i];
    }
  }
};

struct Two {
    int a, b;
    bool operator < (const Two &other) const {
        return a < other.a or (a==other.a and b < other.b);
    }
};

std::vector<Two> getCollisions (std::vector<Sphere>& spheres);

#endif
