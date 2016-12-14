#include <vector>

#include "data_structures/BVH.h"
#include "geometry/Sphere.h"
#include "physics/Intersection.h"
#include "physics/RigidBody.h"

#define LEAF_CAP 5
#define DEPTH_CAP 10
#define NEW_FEATURE 0

using namespace std;
using namespace glm;

BVHNode::BVHNode (const vector<RigidBody*>& objects_, int level) :
  objects(objects_), left(NULL), right(NULL) {
  vector<BoundingBox> objectBoxes;
  vector<BoundingBox> objectBoxesOriginal;
  for (int i = 0; i < objects.size(); ++i) {
    objectBoxes.push_back(objects[i]->getBoundingBox());
    box.merge(objectBoxes[i]);
  }
  objectBoxesOriginal = objectBoxes;

  if (objects.size() < LEAF_CAP || level >= DEPTH_CAP)
    return;

  int axis = 0;
  for (int i = 1; i < 3; ++i) {
    if (box.maxVals[i] - box.minVals[i] > box.maxVals[axis] - box.minVals[axis])
      axis = i;
  }

  auto comp = [&](const BoundingBox& lhs, const BoundingBox& rhs)-> bool {
    return lhs.maxVals[axis] < rhs.maxVals[axis];
  };
  sort(objectBoxes.begin(), objectBoxes.end(), comp);

  float threshold = objectBoxes[objectBoxes.size() / 2].maxVals[axis];

  vector<RigidBody*> leftObjects;
  vector<RigidBody*> rightObjects;

  for (int i = 0; i < objects.size(); ++i) {
    if (objectBoxesOriginal[i].maxVals[axis] < threshold)
      leftObjects.push_back(objects[i]);
    else
      rightObjects.push_back(objects[i]);
  }

  if (leftObjects.size() > 0 && rightObjects.size() > 0) {
    left = new BVHNode(leftObjects, level+1);
    right = new BVHNode(rightObjects, level+1);
  }
}

void BVHNode::getAllBoxes (vector<BoundingBox>& allBoxes) const {
  allBoxes.push_back(box);
  if (left != NULL)
    left->getAllBoxes(allBoxes);
  if (right != NULL)
    right->getAllBoxes(allBoxes);
}

void BVHNode::getAllBoxesDebug (vector<BoundingBox>& allBoxes, vector<bool>& isleft) const {
  if (left != NULL) {
    allBoxes.push_back(left->box);
    isleft.push_back(true);
    left->getAllBoxesDebug(allBoxes, isleft);
  }
  if (right != NULL) {
    allBoxes.push_back(right->box);
    isleft.push_back(false);
    right->getAllBoxesDebug(allBoxes, isleft);
  }
}

#ifdef NEW_FEATURE
bool BVHNode::getIntersection (const Sphere& obj, Intersection& isect) const {
  Intersection tmp;
  if (obj.intersects(box, tmp) == false)
    return false;
  return left->getIntersection(obj, isect) || right->getIntersection(obj, isect);
}

bool BVHNode::getIntersection (const Ray& ray, Intersection& isect) const {
  Intersection tmp;
  if (box.intersects(ray, tmp) == false)
    return false;

  bool result = false;

  if (left == NULL || right == NULL) {
    for (RigidBody* rigid : objects) {
      Intersection rigidIsect;
      if (rigid->intersects(ray, rigidIsect)) {
        result = true;
        if (isect.hit == false || isect.timeHit > rigidIsect.timeHit)
          isect = rigidIsect;
      }
    }
  }
  else {
    if (left != NULL)
      result |= left->getIntersection(ray, isect);
    if (right != NULL)
      result |= right->getIntersection(ray, isect);
  }

  cout << result << endl;

  return result;
}
#endif
