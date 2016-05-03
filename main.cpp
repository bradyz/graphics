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

int main (int argc, char* argv[]) {
  vector<glm::vec4> sphere_vertices;
  vector<glm::uvec3> sphere_faces;
  vector<glm::vec4> sphere_normals;

  LoadOBJ("./obj/sphere.obj", sphere_vertices, sphere_faces, sphere_normals);
  fixSphereVertices(sphere_vertices);

  // vector<glm::vec4> arma_vertices;
  // vector<glm::uvec3> arma_faces;
  // vector<glm::vec4> arma_normals;
  // LoadOBJWithNormals("./obj/armadillo.obj", arma_vertices, arma_faces, arma_normals);

  objects.push_back(Sphere(0.5, glm::vec3(-2.0, 0.0, 0.1)));
  objects[0].velocity = glm::vec3(1.5, 2.5, 0.0);

  objects.push_back(Sphere(0.3, glm::vec3(2.0, 0.0, -0.1)));
  objects[1].velocity = glm::vec3(-1.5, 2.5, 0.0);

  // for (int i = 0; i <= 1000; ++i) {
  //   objects.push_back(Sphere((rand() % 3 + 1) * 0.2, glm::vec3(2.0, i * 1.5, -0.5)));
  //   if (i % 3 == 0)
  //     objects[i].color = RED;
  //   if (i % 3 == 1)
  //     objects[i].color = BLUE;
  //   if (i % 3 == 2)
  //     objects[i].color = WHITE;
  // }

  float bounceK = 0.1;
  glm::vec3 normal;

  normal = normalize(glm::vec3(0.0, 1.0, 0.0));
  planes.push_back(Plane(glm::vec3(0.0, kFloorY-1e-8, 0.0), normal, 10.0, 10.0));
  planes[0].velocity = glm::vec3(0.0, bounceK, 0.0);

  normal = normalize(glm::vec3(-1.0, 1.0, 0.0));
  planes.push_back(Plane(glm::vec3(4.0, 0.0, 0.0), normal, 5.0, 7.0));
  planes[1].velocity = glm::vec3(0.0, bounceK, 0.0);

  normal = normalize(glm::vec3(1.0, 1.0, 0.0));
  planes.push_back(Plane(glm::vec3(-4.0, 0.0, 0.0), normal, 5.0, 7.0));
  planes[2].velocity = glm::vec3(0.0, bounceK, 0.0);

  normal = normalize(glm::vec3(0.0, 1.0, -1.0));
  planes.push_back(Plane(glm::vec3(0.0, 0.0, 4.0), normal, 5.0, 7.0));
  planes[3].velocity = glm::vec3(0.0, bounceK, 0.0);

  normal = normalize(glm::vec3(0.0, 1.0, 1.0));
  planes.push_back(Plane(glm::vec3(0.0, 0.0, -4.0), normal, 5.0, 7.0));
  planes[4].velocity = glm::vec3(0.0, bounceK, 0.0);

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

    for (Plane& plane: planes) {
      if (hack++ == 0)
        continue;

      if (showWire == false)
        phongP.draw(plane.vertices, plane.faces, plane.normals, I);
      else
        wireP.draw(plane.vertices, plane.faces, I, BLUE);

      lineP.drawLineSegment(plane.position, plane.position + plane.normal, RED);
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
          // if (isects[i].hit)
          //   cout << i << " " << isects[i].displacement << endl;

          if (timePaused == false) {
            objects[i].step(forces);
            objects[i].velocity += isects[i].displacement;
          }

          BoundingBox box = objects[i].getBoundingBox();

          if (showWire) {
            if (isects[i].hit)
              lineP.draw(box.getVertices(), box.getEdges(), I, RED);
            else
              lineP.draw(box.getVertices(), box.getEdges(), I, BLUE);
          }

          glm::mat4 toWorld = objects[i].toWorld();

          if (showWire) {
            wireP.draw(sphere_vertices, sphere_faces, toWorld, WHITE);
          }
          else {
            phongP.draw(sphere_vertices, sphere_faces, sphere_normals, toWorld, objects[i].color);
            // shadowP.draw(sphere_vertices, sphere_faces, toWorld);
          }
        }

        break;
      }
    }

    // phongP.draw(arma_vertices, arma_faces, arma_normals, I);
    // shadowP.draw(arma_vertices, arma_faces, I);

    endLoopOpenGL();
  }

  cleanupOpenGL();
}
