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

#include "RandomUtils.h"
#include "OpenGLStuff.h"

#include "Phong.h"
#include "Floor.h"
#include "Shadow.h"
#include "LineSegment.h"
#include "Wire.h"

#include "Intersection.h"
#include "Plane.h"
#include "Sphere.h"
#include "octree.h"

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds milliseconds;

using namespace std;

float BOUNDS = 10.0f;
glm::vec3 minB(-BOUNDS, -BOUNDS, -BOUNDS);
glm::vec3 maxB(BOUNDS, BOUNDS, BOUNDS);

glm::mat4 I;

PhongProgram phongP(&view_matrix, &projection_matrix);
FloorProgram floorP(&view_matrix, &projection_matrix);
ShadowProgram shadowP(&view_matrix, &projection_matrix);
LineSegmentProgram lineP(&view_matrix, &projection_matrix);
WireProgram wireP(&view_matrix, &projection_matrix);

vector<Sphere*> objects;
vector<Plane> planes;
vector<RigidBody*> object_pointers;

vector<glm::vec4> sphere_vertices;
vector<glm::uvec3> sphere_faces;
vector<glm::vec4> sphere_normals;

void drawBoundingBoxes () {

}

void balls () {
  int dim = 8;
  int count = 0;

  for (int i = 0; i < dim; ++i) {
    for (int j = 0; j < dim; ++j) {
      for (int k = 0; k < dim; ++k) {
        float r = 0.01f * (rand() % 100 + 1.0f);
        float g = 0.01f * (rand() % 100 + 1.0f);
        float b = 0.01f * (rand() % 100 + 1.0f);

        float rad = (rand() % 1 + 1) * 0.1f;

        Sphere* tmp = new Sphere(rad, glm::vec3(i * 1.0, j * 1.0 + 10.0f, k * 1.0));
        tmp->color = glm::vec4(r, g, b, 1.0f);
        tmp->mass = 2.0f * glm::clamp(rad, 1.0f, rad);

        objects.push_back(tmp);

        object_pointers.push_back((RigidBody*)objects.back());
      }
    }
  }

  glm::vec3 normal = normalize(glm::vec3(0.0, 1.0, 0.0));
  planes.push_back(Plane(glm::vec3(0.0, kFloorY-2*1e-8, 0.0), normal, 3.85, 3.85));

  normal = normalize(glm::vec3(-1.0, 1.0, 0.0));
  planes.push_back(Plane(glm::vec3(5.0, 0.0, 0.0), normal, 4.0f, 4.0f));

  normal = normalize(glm::vec3(1.0, 1.0, 0.0));
  planes.push_back(Plane(glm::vec3(-5.0, 0.0, 0.0), normal, 4.0f, 4.0f));

  normal = normalize(glm::vec3(0.0, 1.0, -1.0));
  planes.push_back(Plane(glm::vec3(0.0, 0.0, 5.0), normal, 4.0f, 4.0f));

  normal = normalize(glm::vec3(0.0, 1.0, 1.0));
  planes.push_back(Plane(glm::vec3(0.0, 0.0, -5.0), normal, 4.0f, 4.0f));

  vector<glm::vec3> forces;
  forces.push_back(glm::vec3(0.0, -9.8, 0.0));

  Clock::time_point start = Clock::now();
  Clock::time_point t0 = Clock::now();
  Clock::time_point t1 = Clock::now();
  milliseconds ms = chrono::duration_cast<milliseconds>(t1 - t0);
  milliseconds dt = chrono::duration_cast<milliseconds>(t1 - start);

  while (keepLoopingOpenGL()) {
    lineP.drawAxis();

    int hack = 0;

    t0 = t1;
    dt = chrono::duration_cast<milliseconds>(t1 - start);

    if (timePaused == false && false)
      cout << endl << "Current Time: " << dt.count() << endl;

    while (true) {
      t1 = Clock::now();
      ms = chrono::duration_cast<milliseconds>(t1 - t0);

      if (ms.count() > 20) {
        vector<Intersection> isects(objects.size());

        for (int i = 0; i < objects.size(); ++i) {
          for (int j = 0; j < planes.size(); ++j) {
            if (objects[i]->intersects(planes[j], isects[i])) {
          
            }
          }

          for (int j = 0; j < objects.size(); ++j) {
            if (i == j)
              continue;
            if (objects[i]->intersects(*objects[j], isects[i])) {

            }
          }
        }

        for (int i = 0; i < objects.size(); ++i) {
          if (showWire) {
            BoundingBox box = objects[i]->getBoundingBox();
            if (isects[i].hit)
              lineP.draw(box.getVertices(), box.getEdges(), I, RED);
            else
              lineP.draw(box.getVertices(), box.getEdges(), I, BLUE);
          }

          glm::mat4 toWorld = objects[i]->toWorld();

          if (showWire)
            wireP.draw(sphere_vertices, sphere_faces, toWorld, WHITE);
          else {
            phongP.draw(sphere_vertices, sphere_faces, sphere_normals,
                        toWorld, objects[i]->color, glm::vec4(eye, 1.0f));
            // shadowP.draw(sphere_vertices, sphere_faces, toWorld);
          }
        }

        if (showFloor && showWire == false) {
          for (Plane& plane: planes) {
            glm::vec4 color(0.72f, 0.60f, 0.41f, 1.0f);
            phongP.draw(plane.vertices, plane.faces, plane.normals,
                I, color, glm::vec4(eye, 1.0f));
          }
        }

        if (showWire) {
          OctTreeNode root(object_pointers, BoundingBox(minB, maxB));

          vector<BoundingBox> octtreeBoxes;

          root.getAllBoxes(octtreeBoxes);

          for (BoundingBox& box: octtreeBoxes) {
            vector<glm::vec4> vertices = box.getVertices();
            vector<glm::uvec2> edges = box.getEdges();

            for (glm::uvec2 edge: edges) {
              glm::vec3 u = glm::vec3(vertices[edge[0]]);
              glm::vec3 v = glm::vec3(vertices[edge[1]]);
              lineP.drawLineSegment(u, v, RED);
            }
          }
        }

        for (int i = 0; i < objects.size(); ++i) {
          if (timePaused == false) {
            objects[i]->velocity += isects[i].displacement;
            objects[i]->step(forces);
          }
        }

        break;
      }
    }

    endLoopOpenGL();
  }
}

int main (int argc, char* argv[]) {
  LoadOBJ("./obj/sphere.obj", sphere_vertices, sphere_faces, sphere_normals);
  fixSphereVertices(sphere_vertices);

  initOpenGL();

  phongP.setup();
  floorP.setup();
  shadowP.setup();
  lineP.setup();
  wireP.setup();

  balls();

  cleanupOpenGL();
}
