#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>

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

int main(int argc, char* argv[]) {
  vector<glm::vec4> sphere_vertices;
  vector<glm::uvec3> sphere_faces;
  vector<glm::vec4> sphere_normals;

  LoadOBJ("./obj/sphere.obj", sphere_vertices, sphere_faces, sphere_normals);
  fixSphereVertices(sphere_vertices);

  // vector<glm::vec4> arma_vertices;
  // vector<glm::uvec3> arma_faces;
  // vector<glm::vec4> arma_normals;
  //
  // LoadOBJWithNormals("./obj/armadillo.obj", arma_vertices, arma_faces, arma_normals);

  objects.push_back(Sphere(0.5, glm::vec3(-2.0, 0.0, 0.0)));
  objects[0].velocity = glm::vec3(1.5, 2.5, 0.0);

  objects.push_back(Sphere(0.8, glm::vec3(2.0, 0.0, 0.0)));
  objects[1].velocity = glm::vec3(-1.5, 2.5, 0.0);

  glm::vec3 normal;

  normal = normalize(glm::vec3(0.0, 1.0, 0.0));
  planes.push_back(Plane(glm::vec3(0.0, kFloorY, 0.0), normal, 10.0, 10.0));
  planes[0].velocity = glm::vec3(0.0, 2.5, 0.0);

  normal = normalize(glm::vec3(-1.0, 0.0, 0.0));
  planes.push_back(Plane(glm::vec3(4.0, 0.0, 0.0), normal, 2.0, 2.0));
  planes[1].velocity = glm::vec3(0.0, 1.0, 0.0);

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

    for (Plane& plane: planes) {
      wireP.draw(plane.vertices, plane.faces, I, BLUE);
    }

    if (showWire == false)
      floorP.draw();

    t0 = t1;
    dt = chrono::duration_cast<milliseconds>(t1 - start);

    if (timePaused == false)
      cout << endl << "Current Time: " << dt.count() << endl;

    while (true) {
      t1 = Clock::now();
      ms = chrono::duration_cast<milliseconds>(t1 - t0);

      if (ms.count() > 10) {
        vector<Intersection> isects(objects.size());

        for (int i = 0; i < objects.size(); ++i) {
          for (int j = 0; j < objects.size(); ++j) {
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
          if (isects[i].hit)
            cout << i << " " << isects[i].displacement << endl;

          if (timePaused == false) {
            objects[i].step(forces);
            objects[i].velocity += isects[i].displacement;
          }

          BoundingBox box = objects[i].getBoundingBox();

          if (isects[i].hit)
            lineP.draw(box.getVertices(), box.getEdges(), I, RED);
          else
            lineP.draw(box.getVertices(), box.getEdges(), I, BLUE);

          glm::mat4 toWorld = objects[i].toWorld();

          if (showWire) {
            wireP.draw(sphere_vertices, sphere_faces, toWorld, BLUE);
          }
          else {
            phongP.draw(sphere_vertices, sphere_faces, sphere_normals, toWorld);
            shadowP.draw(sphere_vertices, sphere_faces, toWorld);
          }
        }

        break;
      }
    }

    // phongP.draw(arma_vertices, arma_faces, arma_normals, I);
    // shadowP.draw(arma_vertices, arma_faces, I);
    // phongP.draw(plane.vertices, plane.faces, plane.normals, I);

    endLoopOpenGL();
  }

  cleanupOpenGL();
}
