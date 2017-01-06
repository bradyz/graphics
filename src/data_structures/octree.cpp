#include <vector>
#include <set>
#include <iostream>
#include <cstdlib>
#include <time.h>

#include "data_structures/octree.h"
#include "geometry/Sphere.h"
#include "physics/RigidBody.h"
#include "geometry/BoundingBox.h"

using namespace std;

const int LEAF_CAP = 5;
const int DEPTH_CAP = 4;

OctTreeNode::OctTreeNode (const vector<RigidBody*>& objects, int level,
                          BoundingBox bbox) : box(bbox) {
  for (int i = 0; i < 8; i++)
    cells[i] = NULL;

  float dx = (box.maxVals[0] - box.minVals[0]) / 2.0f;

  // Initial box will be created and made uniform.
  if (level == 0) {
    for (RigidBody *object : objects)
      this->box.add(object->position);

    for (int i = 0; i < 3; i++)
      dx = max(dx, (this->box.maxVals[i] - this->box.minVals[i]) / 2.0f);

    for (int i = 0; i < 3; i++)
      this->box.maxVals[i] = this->box.minVals[i] + dx;
  }

  // Make sure bounds are uniform.
  for (int i = 1; i < 3; i++) {
    float d1 = box.maxVals[i-1] - box.minVals[i-1];
    float d2 = box.maxVals[i] - box.minVals[i];
    assert(abs(d1 - d2) < 1e-5);
  }

  // Leaf case.
  if (objects.size() <= LEAF_CAP || level > DEPTH_CAP) {
    for (RigidBody *object : objects)
      this->objects.push_back(object);
    return;
  }

  vector<RigidBody*> items[8];
  BoundingBox boxes[8];

  int idx = 0;
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      for (int k = 0; k < 2; k++) {
        glm::vec3 tmpMin = box.minVals + glm::vec3(i * dx, j * dx, k * dx);
        boxes[idx++] = BoundingBox(tmpMin, tmpMin + dx);
      }
    }
  }

  for (RigidBody *obj : objects) {
    for (int i = 0; i < 8; i++) {
      if (boxes[i].intersects(obj->position))
        items[i].push_back(obj);
    }
  }

  for (int i = 0; i < 8; i++) {
    if (items[i].size() > 0)
      cells[i] = new OctTreeNode(items[i], level+1, boxes[i]);
  }
}

vector<const OctTreeNode*> OctTreeNode::getAllNodes () const {
  vector<const OctTreeNode*> result;
  result.push_back(this);

  for (int i = 0; i < 8; i++) {
    if (this->cells[i] != NULL) {
      for (const OctTreeNode *child : this->cells[i]->getAllNodes())
        result.push_back(child);
    }
  }

  return result;
}

bool OctTreeNode::isLeaf () const {
  return (this->objects.size() > 0);
}
