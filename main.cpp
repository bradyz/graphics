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

    if (timePaused == false)
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
          cout << i << " " << objects.size() << endl;
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

void boids () {
  BOUNDS = 15.0f;
  glm::vec3 minB(-BOUNDS, -BOUNDS, -BOUNDS);
  glm::vec3 maxB(BOUNDS, BOUNDS, BOUNDS);

  BoundingBox theBounds(minB, maxB);

  int dim = 6;
  int count = 0;

  for (int i = 0; i < dim; ++i) {
    for (int j = 0; j < dim; ++j) {
      for (int k = 0; k < dim; ++k) {
        float r = 0.01f * (rand() % 100 + 1.0f);
        float g = 0.01f * (rand() % 100 + 1.0f);
        float b = 0.01f * (rand() % 100 + 1.0f);

        float rad = (rand() % 3 + 1) * 0.1f;

        Sphere* tmp = new Sphere(rad, glm::vec3(i * 1.0, j * 1.0 + 1.0f, k * 1.0));
        tmp->color = glm::vec4(r, g, b, 1.0f);
        tmp->velocity = glm::vec3(r * 3.0f, g * 3.0f, b * 3.0f);
        tmp->velocity.x -= 1.5f;
        tmp->velocity.y -= 1.5f;
        tmp->velocity.z -= 1.5f;

        objects.push_back(tmp);

        object_pointers.push_back((RigidBody*)objects.back());
      }
    }
  }

  while (keepLoopingOpenGL()) {
    lineP.drawAxis();

    if (hasFood) {
      glm::vec3 tmp = glm::normalize(foodPos - eye);
      glm::vec3 tmp1 = glm::vec3(0, 0, 0) - eye;
      float t = glm::length(glm::dot(tmp, tmp1) * tmp);
      foodPos = eye + tmp * t;
    }

    int leader = rand() % objects.size();

    for (int i = 0; i < 15; ++i) {
      int r = rand() % objects.size();
      objects[r]->velocity.x += ((rand() % 11) - 5) * 2.5f;
      objects[r]->velocity.y += ((rand() % 11) - 5) * 2.5f;
      objects[r]->velocity.z += ((rand() % 11) - 5) * 2.5f;
    }

    vector<glm::vec3> changeV;

    for (int i = 0; i < objects.size(); ++i) {
      glm::vec3 centerMass;
      glm::vec3 getAway;

      multimap<float, Sphere*> distMap;

      for (int j = 0; j < objects.size(); ++j) {
        if (i == j)
          continue;
        centerMass += objects[j]->position;
        float dist = glm::length2(objects[j]->position - objects[i]->position);
        if (dist < (objects[i]->radius + objects[j]->radius) * 2.5f)
          getAway += 2.5f * dist * glm::normalize(objects[i]->position - objects[j]->position);
        distMap.insert(pair<float, Sphere*>(dist, objects[j]));
      }

      centerMass /= (objects.size() - 1);

      int numNeighbors = objects.size() / 10;
      glm::vec3 avgV;
      glm::vec3 avgP;

      int num = 0;
      for (auto neighbor: distMap) {
        if (++num > numNeighbors)
          break;
        avgV += neighbor.second->velocity;
        avgP += neighbor.second->position;
      }

      avgV /= static_cast<float>(numNeighbors);
      avgP /= static_cast<float>(numNeighbors);

      glm::vec3 dV1 = avgP - objects[i]->position;
      glm::vec3 dV2 = centerMass - objects[i]->position;
      glm::vec3 totaldV = avgV * 0.10f + dV1 * 0.18f + dV2 * 0.28f + getAway;

      if (rand() % 3 == 0)
        totaldV += objects[leader]->velocity * 0.03f;

      if (hasFood)
        totaldV += 0.40f * (foodPos - objects[i]->position);

      if (theBounds.intersects(objects[i]->position) == false) {
        glm::vec3 goTo;
        int idx = rand() % 8;
        int cur = 0;
        for (int x = 0; x < 2; ++x) {
          for (int y = 0; y < 2; ++y) {
            for (int z = 0; z < 2; ++z) {
              if (cur++ == idx)
                goTo = minB + BOUNDS * glm::vec3(x, y, z);
            }
          }
        }
        totaldV += 0.3f * BOUNDS * glm::normalize(goTo - objects[i]->position);
      }

      changeV.push_back(totaldV);
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
        objects[i]->velocity += changeV[i];
        objects[i]->velocity *= 0.99f;
        objects[i]->step(vector<glm::vec3>());
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

    if (hasFood) {
      glm::mat4 T = glm::translate(foodPos);
      glm::mat4 S = glm::scale(glm::vec3(0.5f, 0.5f, 0.5f));
      phongP.draw(sphere_vertices, sphere_faces, sphere_normals,
                  T * S, RED, glm::vec4(eye, 1.0f));
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

  if (string(argv[1]) == "balls")
    balls();
  else
    boids();

  cleanupOpenGL();
}
