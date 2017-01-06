#include <algorithm>
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <Eigen/SparseCore>

#include "geometry/Geometry.h"
#include "geometry/Triangle.h"
#include "helpers/RandomUtils.h"
#include "render/LineSegment.h"
#include "render/OpenGLStuff.h"
#include "render/Phong.h"
#include "render/Wire.h"

using namespace std;
using namespace glm;

const size_t GRID_SIZE = 25;

mat4 I;

PhongProgram phongP(&view_matrix, &projection_matrix);
LineSegmentProgram lineP(&view_matrix, &projection_matrix);
WireProgram wireP(&view_matrix, &projection_matrix);

vector<Geometry> objects;

vector<vec4> bunny_vertices;
vector<uvec3> bunny_faces;
vector<vec4> bunny_normals;

int x_pos = 0;
int x_dir = 1;
int y_pos = 0;

Eigen::SparseMatrix<double> adjacencyMatrix(size_t n, const vector<uvec3> &faces) {
  Eigen::SparseMatrix<double> A(n, n);
  A.reserve(Eigen::VectorXi::Constant(n, n));
  for (const uvec3 &face : faces) {
    for (int i = 0; i < 3; i++) {
      for (int j = i + 1; j < 3; j++) {
        A.coeffRef(face[i], face[j]) += 1.0;
        A.coeffRef(face[j], face[i]) += 1.0;
      }
    }
  }
  A.makeCompressed();
  return A;
}

Eigen::MatrixXd convertToEigenMatrix(const vector<vec4> &points) {
  size_t n = points.size();
  Eigen::MatrixXd A(n, 4);
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < 4; j++)
      A(i, j) = points[i][j];
  }
  return A;
}

vector<vec4> convertToVectorVec4(Eigen::MatrixXd &A, bool debug=false) {
  size_t n = A.rows();
  vector<vec4> points(n);
  for (int i = 0; i < A.rows(); i++) {
    A.row(i) /= A(i, 3);
    for (int j = 0; j < 4; j++)
      points[i][j] = A(i, j);
  }
  return points;
}

void addToBack(const vector<vec4>& v, const vector<uvec3> &f) {
  Geometry& previous = objects.back();

  objects.push_back(Geometry(v, f));

  float y = 0.0f;

  x_pos += x_dir;
  if (x_pos >= GRID_SIZE) {
    x_dir = 0.0f;
    y_pos++;
    y = 1.5f;
  }
  else if (x_pos <= 0) {
    x_dir = 0.0f;
    y_pos++;
    y = 1.5f;
  }

  objects.back().toWorld = translate(I, vec3(x_dir * 1.5f, 0.0f, y)) * previous.toWorld;

  // Why do I have to set the entry manually? Check defaults.
  objects.back().toWorld[3][3] = 1.0f;

  if (x_pos >= GRID_SIZE) {
    x_dir = -1.0f;
  }
  else if (x_pos <= 0) {
    x_dir = 1.0f;
  }
}

int main (int argc, char* argv[]) {
  LoadOBJ("./obj/bunny.obj", bunny_vertices, bunny_faces, bunny_normals);

  initOpenGL();

  camera_distance = 1.0f;
  eye = glm::vec3(1.0f, 1.0f, 1.0f);
  showWire = false;

  phongP.setup();
  lineP.setup();
  wireP.setup();

  objects.push_back(Geometry(bunny_vertices, bunny_faces));
  objects.back().toWorld = scale(I, vec3(10.0f, 10.0f, 10.0f));

  Eigen::SparseMatrix<double> A = adjacencyMatrix(bunny_vertices.size(), bunny_faces);
  Eigen::MatrixXd P = convertToEigenMatrix(bunny_vertices);
  Eigen::MatrixXd AP = P;

  int num = 0;
  for (num = 0; num < 625; num++) {
      AP = A * AP;
      addToBack(convertToVectorVec4(AP), bunny_faces);
  }

  while (keepLoopingOpenGL()) {
    lineP.drawAxis();

    if (do_action) {
      num++;
      AP = A * AP;
      addToBack(convertToVectorVec4(AP, true), bunny_faces);
      do_action = false;
    }

    for (int i = 0; i < objects.size(); i++) {
      const vec4 color = jet((static_cast<double>(i) / objects.size() - 0.5) * 2.0);
      const Geometry &geom = objects[i];
      if (showWire) {
        wireP.draw(geom.vertices, geom.faces, geom.toWorld, color);
      }
      else {
        phongP.draw(geom.vertices, geom.faces, geom.normals,
                    geom.toWorld, color, vec4(eye, 1.0f));
      }
    }

    endLoopOpenGL();
  }

  cleanupOpenGL();
}
