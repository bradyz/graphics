#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <random>

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

glm::mat4 I;

PhongProgram phongP(&view_matrix, &projection_matrix);
FloorProgram floorP(&view_matrix, &projection_matrix);
ShadowProgram shadowP(&view_matrix, &projection_matrix);
LineSegmentProgram lineP(&view_matrix, &projection_matrix);
WireProgram wireP(&view_matrix, &projection_matrix);

vector<Sphere> objects;
vector<Plane> planes;

void broadPhase () {

}

int main (int argc, char* argv[]) {
  vector<glm::vec4> sphere_vertices;
  vector<glm::uvec3> sphere_faces;
  vector<glm::vec4> sphere_normals;

  LoadOBJ("./obj/sphere.obj", sphere_vertices, sphere_faces, sphere_normals);
  fixSphereVertices(sphere_vertices);

  int num = 0;

  int dim = 8;

  for (int i = 0; i < dim; ++i) {
    for (int j = 0; j < dim; ++j) {
      for (int k = 0; k < dim; ++k) {
        float x = rand() % 3 - 1.5f;
        float z = rand() % 3 - 1.5f;

        float r = 0.01f * (rand() % 100 + 1.0f);
        float g = 0.01f * (rand() % 100 + 1.0f);
        float b = 0.01f * (rand() % 100 + 1.0f);

        float rad = (rand() % 3 + 1) * 0.15f;

        objects.push_back(Sphere(rad, glm::vec3(i * 2.0, j * 2.0 + 10.0f, k * 2.0)));
        objects[num].color = glm::vec4(r, g, b, 1.0f);
        objects[num++].mass = glm::clamp(rad, 1.0f, rad);
      }
    }
  }

  glm::vec3 normal = normalize(glm::vec3(0.0, 1.0, 0.0));
  planes.push_back(Plane(glm::vec3(0.0, kFloorY-2*1e-8, 0.0), normal, 100.0, 100.0));

  normal = normalize(glm::vec3(-1.0, 1.0, 0.0));
  planes.push_back(Plane(glm::vec3(4.0, 0.0, 0.0), normal, 100, 100));

  normal = normalize(glm::vec3(1.0, 1.0, 0.0));
  planes.push_back(Plane(glm::vec3(-4.0, 0.0, 0.0), normal, 100, 100));

  normal = normalize(glm::vec3(0.0, 1.0, -1.0));
  planes.push_back(Plane(glm::vec3(0.0, 0.0, 4.0), normal, 100, 100));

  normal = normalize(glm::vec3(0.0, 1.0, 1.0));
  planes.push_back(Plane(glm::vec3(0.0, 0.0, -4.0), normal, 100, 100));

  vector<glm::vec3> forces;
  forces.push_back(glm::vec3(0.0, -9.8, 0.0));

  initOpenGL();

  phongP.setup();
  floorP.setup();
  shadowP.setup();
  lineP.setup();
  wireP.setup();

  Clock::time_point start = Clock::now();
  Clock::time_point t0 = Clock::now();
  Clock::time_point t1 = Clock::now();
  milliseconds ms = chrono::duration_cast<milliseconds>(t1 - t0);
  milliseconds dt = chrono::duration_cast<milliseconds>(t1 - start);

  while (keepLoopingOpenGL()) {
    lineP.drawAxis();

    int hack = 0;
    // if (showWire == false)
    //   floorP.draw();

    t0 = t1;
    dt = chrono::duration_cast<milliseconds>(t1 - start);

    if (timePaused == false)
      cout << endl << "Current Time: " << dt.count() << endl;

    while (true) {
      t1 = Clock::now();
      ms = chrono::duration_cast<milliseconds>(t1 - t0);

      if (ms.count() > 20) {
        vector<Intersection> isects(objects.size());

        for (int i = 0; i < objects.size(); ++i) {
          for (int j = 0; j < planes.size(); ++j) {
            if (objects[i].intersects(planes[j], isects[i])) {

            }
          }

          for (int j = 0; j < objects.size(); ++j) {
            if (i == j)
              continue;
            if (objects[i].intersects(objects[j], isects[i])) {

            }
          }
        }

        for (int i = 0; i < objects.size(); ++i) {
          if (timePaused == false) {
            objects[i].velocity += isects[i].displacement;
            objects[i].step(forces);
          }

          if (showWire) {
            BoundingBox box = objects[i].getBoundingBox();
            if (isects[i].hit)
              lineP.draw(box.getVertices(), box.getEdges(), I, RED);
            else
              lineP.draw(box.getVertices(), box.getEdges(), I, BLUE);
          }

          glm::mat4 toWorld = objects[i].toWorld();

          if (showWire)
            wireP.draw(sphere_vertices, sphere_faces, toWorld, WHITE);
          else {
            phongP.draw(sphere_vertices, sphere_faces, sphere_normals,
                        toWorld, objects[i].color, glm::vec4(eye, 1.0f));
            // shadowP.draw(sphere_vertices, sphere_faces, toWorld);
          }
        }

        for (Plane& plane: planes) {
          glm::vec4 color(0.0f, 1.0f, 1.0f, 1.0f);

          if (showWire)
            wireP.draw(plane.vertices, plane.faces, I, BLUE);
          else {
            phongP.draw(plane.vertices, plane.faces, plane.normals,
                        I, color, glm::vec4(eye, 1.0f));
            
          }

          lineP.drawLineSegment(plane.position, plane.position + plane.normal, RED);
        }

        break;
      }
    }

    endLoopOpenGL();
  }

  cleanupOpenGL();
}
