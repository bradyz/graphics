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

  OctTreeNode (const std::vector<RigidBody*>& newObjects,
               int level=0, BoundingBox bbox=BoundingBox());

  std::vector<const OctTreeNode*> getAllNodes () const;

  bool isLeaf () const;

  ~OctTreeNode () {
    for (int i = 0; i < 8; ++i) {
      if (cells[i] != NULL)
        delete cells[i];
    }
  }
};

#endif
