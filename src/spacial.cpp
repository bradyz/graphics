#include <algorithm>
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "data_structures/BVH.h"
#include "data_structures/octree.h"
#include "geometry/Geometry.h"
#include "geometry/Triangle.h"
#include "geometry/Sphere.h"
#include "helpers/RandomUtils.h"
#include "render/LineSegment.h"
#include "render/OpenGLStuff.h"
#include "render/Phong.h"
#include "render/Wire.h"

using namespace std;
using namespace glm;

const vec3 SCALE = vec3(20.0f, 20.0f, 20.0f);
const vec3 TRANSLATE = vec3(0.2f, 0.0f, 0.0f);

const mat4 I;
const mat4 POSITION_A = scale(I, SCALE) * translate(I, TRANSLATE);
const mat4 POSITION_B = translate(I, -TRANSLATE) * scale(I, SCALE);

const float SPHERE_SIZE = 0.02f;

PhongProgram phongP(&view_matrix, &projection_matrix);
LineSegmentProgram lineP(&view_matrix, &projection_matrix);
WireProgram wireP(&view_matrix, &projection_matrix);

vector<vec4> bunny_vertices;
vector<uvec3> bunny_faces;
vector<vec4> bunny_normals;

vector<vec4> sphere_vertices;
vector<uvec3> sphere_faces;
vector<vec4> sphere_normals;

vector<RigidBody*> bunny_octtree_mesh;
vector<RigidBody*> bunny_bvh_mesh;

OctTreeNode *octtree;
BVHNode *bvh;

void setupModels () {
  for (const vec4 &vertex : bunny_vertices) {
    bunny_octtree_mesh.push_back(new Sphere(SPHERE_SIZE, POSITION_A * vertex));
    bunny_bvh_mesh.push_back(new Sphere(SPHERE_SIZE, POSITION_B * vertex));
  }

  octtree = new OctTreeNode(bunny_octtree_mesh);
  bvh = new BVHNode(bunny_bvh_mesh);
}

void setupOpenGL () {
  initOpenGL();

  camera_distance = 1.0f;
  eye = glm::vec3(1.0f, 1.0f, 1.0f);
  showWire = false;

  phongP.setup();
  lineP.setup();
  wireP.setup();
}

int main (int argc, char* argv[]) {
  LoadOBJ("./obj/bunny.obj", bunny_vertices, bunny_faces, bunny_normals);
  LoadOBJ("./obj/sphere.obj", sphere_vertices, sphere_faces, sphere_normals);

  setupOpenGL();
  setupModels();

  while (keepLoopingOpenGL()) {
    lineP.drawAxis();

    for (RigidBody *object : bunny_octtree_mesh) {
      phongP.draw(sphere_vertices, sphere_faces, sphere_normals,
                  dynamic_cast<Sphere*>(object)->toWorld(),
                  object->color, glm::vec4(eye, 1.0f));
    }

    for (const OctTreeNode *node : octtree->getAllNodes()) {
      if (!node->isLeaf())
        lineP.drawBoundingBox(node->box, RED);
    }

    for (const OctTreeNode *node : octtree->getAllNodes()) {
      if (node->isLeaf())
        lineP.drawBoundingBox(node->box, BLUE);
    }

    endLoopOpenGL();
  }

  cleanupOpenGL();
}
