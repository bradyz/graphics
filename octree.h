#ifndef OCTREE_H
#define OCTREE_H

#include <vector>

#include "Sphere.h"

struct Two {
    int a, b;
    bool operator < (const Two &other) const {
        return a < other.a or (a==other.a and b < other.b);
    }
};

std::vector<Two> getCollisions (std::vector<Sphere>& spheres);

#endif
