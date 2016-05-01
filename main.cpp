#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <GL/glew.h>

#include <glm/glm.hpp>

#include "RandomUtils.h"
#include "OpenGLStuff.h"

#include "Sphere.h"
#include "Phong.h"
#include "Floor.h"
#include "Shadow.h"
#include "LineSegment.h"

using namespace std;

const float INF = 1e9;

vector<Sphere> objects;

void updatePosition () {
  
}

PhongProgram phongP(&view_matrix, &projection_matrix);
FloorProgram floorP(&view_matrix, &projection_matrix);
ShadowProgram shadowP(&view_matrix, &projection_matrix);
LineSegmentProgram lineP(&view_matrix, &projection_matrix);

int main(int argc, char* argv[]) {
  initOpenGL();

  phongP.setup();
  floorP.setup();
  shadowP.setup();
  lineP.setup();

  vector<glm::vec4> sphere_vertices;
  vector<glm::uvec3> sphere_faces;
  vector<glm::vec4> sphere_normals;

  LoadOBJ("./obj/sphere.obj", sphere_vertices, sphere_faces);
  sphere_normals = getVertexNormals(sphere_vertices, sphere_faces);

  vector<glm::vec4> armadillo_vertices;
  vector<glm::uvec3> armadillo_faces;
  vector<glm::vec4> armadillo_normals;

  LoadOBJWithNormals("./obj/armadillo.obj", armadillo_vertices, armadillo_faces,
                     armadillo_normals);

  objects.push_back(Sphere(5, glm::vec3(0.0, 1.0, 0.0)));
  objects.push_back(Sphere(3, glm::vec3(0.5, 1.0, 0.0)));

  while (keepLoopingOpenGL()) {
    glm::mat4 model = glm::mat4(1.0f);

    phongP.draw(sphere_vertices, sphere_faces, sphere_normals, model);
    shadowP.draw(sphere_vertices, sphere_faces, model);

    phongP.draw(armadillo_vertices, armadillo_faces, armadillo_normals, model);
    shadowP.draw(armadillo_vertices, armadillo_faces, model);

    lineP.drawAxis();

    floorP.draw();

    endLoopOpenGL();
  }

  cleanupOpenGL();
}
