#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <random>
#include <map>
#include <utility>

#include <GL/glew.h>

#include <glm/glm.hpp>

#include "helpers/RandomUtils.h"

#include "data_structures/octree.h"

#include "geometry/Plane.h"
#include "geometry/Sphere.h"

#include "physics/Intersection.h"
#include "physics/Spring.h"

#include "render/Floor.h"
#include "render/LineSegment.h"
#include "render/OpenGLStuff.h"
#include "render/Phong.h"
#include "render/Shadow.h"
#include "render/Wire.h"

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds milliseconds;

using namespace std;

glm::mat4 I;

PhongProgram phongP(&view_matrix, &projection_matrix);
FloorProgram floorP(&view_matrix, &projection_matrix);
ShadowProgram shadowP(&view_matrix, &projection_matrix);
LineSegmentProgram lineP(&view_matrix, &projection_matrix);
WireProgram wireP(&view_matrix, &projection_matrix);

vector<Sphere*> spheres;
vector<Plane> planes;
vector<RigidBody*> rigid_bodies;
vector<Spring*> springs;

vector<glm::vec4> sphere_vertices;
vector<glm::uvec3> sphere_faces;
vector<glm::vec4> sphere_normals;

const glm::vec4 FLOOR_COLOR(0.72f, 0.60f, 0.41f, 1.0f);
const glm::vec3 GRAVITY(0.0, -1.0, 0.0);

void setupOpengl() {
  initOpenGL();

  phongP.setup();
  floorP.setup();
  shadowP.setup();
  lineP.setup();
  wireP.setup();
}

void setupCloth () {
  LoadOBJ("./obj/sphere.obj", sphere_vertices, sphere_faces, sphere_normals);
  fixSphereVertices(sphere_vertices);

  Sphere* sphere;
  Spring *spring;

  sphere = new Sphere(0.1, glm::vec3(1.0, 3.0, 1.0));
  spheres.push_back(sphere);

  sphere = new Sphere(0.1, glm::vec3(1.0, 2.0, 1.0));
  spheres.push_back(sphere);

  spring = new Spring(*spheres[0], *spheres[1]);
  springs.push_back(spring);

  for (Sphere* s : spheres)
    rigid_bodies.push_back((RigidBody*)s);

  planes.push_back(Plane(glm::vec3(0.0, kFloorY-1e-3, 0.0)));
}

void cloth() {
  lineP.drawAxis();

  vector<glm::vec3> forces;

  for (Spring *spring : springs) {
    spring->step();
  }

  for (const Sphere *sphere : spheres) {
    glm::mat4 toWorld = sphere->toWorld();

    if (showWire) {
      wireP.draw(sphere_vertices, sphere_faces, toWorld, WHITE);
    }
    else {
      phongP.draw(sphere_vertices, sphere_faces, sphere_normals,
                  toWorld, sphere->color, glm::vec4(eye, 1.0f));
      shadowP.draw(sphere_vertices, sphere_faces, toWorld);
    }
  }

  for (const Plane& plane: planes) {
    if (showFloor && showWire == false) {
      phongP.draw(plane.vertices, plane.faces, plane.normals, I,
                  FLOOR_COLOR, glm::vec4(eye, 1.0f));
    }
  }

  for (int i = 1; i < spheres.size(); i++) {
    Sphere *sphere = spheres[i];

    sphere->applyForce(GRAVITY);

    if (!timePaused) {
      sphere->position += sphere->stepOffset(forces);
    }
  }
}

int main (int argc, char* argv[]) {
  setupOpengl();
  setupCloth();
  while (keepLoopingOpenGL()) {
    cloth();
    endLoopOpenGL();
  }
  cleanupOpenGL();
}
