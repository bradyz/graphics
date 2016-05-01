#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <GL/glew.h>

#include <glm/glm.hpp>

#include "RandomUtils.h"
#include "OpenGLStuff.h"

#include "Phong.h"
#include "Floor.h"
#include "Shadow.h"
#include "LineSegment.h"

#include "Plane.h"
#include "Sphere.h"

using namespace std;

glm::mat4 I;

PhongProgram phongP(&view_matrix, &projection_matrix);
FloorProgram floorP(&view_matrix, &projection_matrix);
ShadowProgram shadowP(&view_matrix, &projection_matrix);
LineSegmentProgram lineP(&view_matrix, &projection_matrix);

vector<Sphere> objects;

void updatePosition () {
  
}

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

  objects.push_back(Sphere(1.0, glm::vec3(0.0, 0.0, 0.0)));

  // glm::vec3 normal = normalize(glm::vec3(1.0, 1.0, -0.5));
  // Plane plane(glm::vec3(0.0, 0.0, 0.0), normal, 100, 100);
  
  vector<glm::vec3> forces;
  forces.push_back(glm::vec3(0.0, 0.0, 9.8));

  initOpenGL();

  phongP.setup();
  floorP.setup();
  shadowP.setup();
  lineP.setup();

  while (keepLoopingOpenGL()) {

    floorP.draw();
    lineP.drawAxis();

    // phongP.draw(arma_vertices, arma_faces, arma_normals, I);
    // shadowP.draw(arma_vertices, arma_faces, I);
    // phongP.draw(plane.vertices, plane.faces, plane.normals, I);

    for (Sphere& sphere: objects) {
      sphere.step(forces);

      glm::mat4 toWorld = sphere.toWorld();

      phongP.draw(sphere_vertices, sphere_faces, sphere_normals, toWorld);
      shadowP.draw(sphere_vertices, sphere_faces, toWorld);

      BoundingBox box = sphere.getBoundingBox();
      lineP.draw(box.getVertices(), box.getEdges(), I, BLUE);
    }

    endLoopOpenGL();
  }

  cleanupOpenGL();
}
