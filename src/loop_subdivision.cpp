#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_set>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "data_structures/BVH.h"
#include "data_structures/octree.h"
#include "geometry/Geometry.h"
#include "geometry/Plane.h"
#include "geometry/Sphere.h"
#include "geometry/Triangle.h"
#include "helpers/RandomUtils.h"
#include "physics/Intersection.h"
#include "render/Floor.h"
#include "render/LineSegment.h"
#include "render/OpenGLStuff.h"
#include "render/Phong.h"
#include "render/Shadow.h"
#include "render/Wire.h"

using namespace std;
using namespace glm;

mat4 I;

PhongProgram phongP(&view_matrix, &projection_matrix);
FloorProgram floorP(&view_matrix, &projection_matrix);
ShadowProgram shadowP(&view_matrix, &projection_matrix);
LineSegmentProgram lineP(&view_matrix, &projection_matrix);
WireProgram wireP(&view_matrix, &projection_matrix);

vector<Geometry> objects;
vector<Plane> planes;

vector<vec4> cube_vertices;
vector<uvec3> cube_faces;
vector<vec4> cube_normals;

vec4 barycenter (const vec4& a, const vec4& b, const vec4& c) {
  vec4 bary(0.0f, 0.0f, 0.0f, 0.0f);
  bary += a;
  bary += b;
  bary += c;
  return bary / 3.0f;
}

void subdivide (vector<vec4>& vertices, vector<uvec3>& faces,
                vector<vec4>& v, vector<uvec3>& f) {
  f.clear();
  v = vertices;

  for (const uvec3 &face : faces)
    f.push_back(face);

  for (const uvec3 &face : faces) {
    vec4 barycenter;
    for (int i = 0; i < 3; ++i)
      barycenter += vertices[face[i]] / 3.0f;

    int barycenter_idx = v.size();
    v.push_back(barycenter);

    for (int i = 0; i < 3; ++i) {
      int a = face[i];
      int b = face[(i+1) % 3];

      vec4 edge_avg = (vertices[a] + vertices[b]) / 2.0f;
      int edge_avg_idx = v.size();
      v.push_back(edge_avg);

      uvec3 new_face;
      new_face[0] = edge_avg_idx;
      new_face[1] = barycenter_idx;
      new_face[2] = a;

      f.push_back(new_face);
    }
  }

  fixDuplicateVertices(v, f);
}

void loop_subdivide (vector<vec4>& vertices, vector<uvec3>& faces,
                     vector<vec4>& v, vector<uvec3>& f) {
  v = vertices;
  f.clear();

  // Map from vertex index to face indices containing that vertex.
  map<int, vector<int> > vertexFaces;

  for (int i = 0; i < faces.size(); ++i)
    for (int j = 0; j < 3; ++j)
      vertexFaces[faces[i][j]].push_back(i);

  for (int i = 0; i < faces.size(); ++i) {
    int offset = v.size();
    bool is_manifold_edge = true;

    for (int j = 0; j < 3; ++j) {
      // Want to find the d.
      //      a
      //    / | \
      //   /  e  \
      //  /   |   \
      // d----b----c
      int a = faces[i][j];
      int b = faces[i][(j+1) % 3];
      int c = faces[i][(j+2) % 3];
      int d = -1;

      // find the other triangle that shares this edge
      for (int neighbor_idx : vertexFaces[faces[i][j]]) {
        const uvec3& face = faces[neighbor_idx];

        bool a_found = false;
        bool b_found = false;
        bool c_found = false;

        for (int k = 0; k < 3; ++k) {
          a_found |= (a == face[k]);
          b_found |= (b == face[k]);
          c_found |= (c == face[k]);
        }

        if (a_found && b_found && !c_found) {
          for (int k = 0; k < 3; ++k) {
            if (face[k] != a && face[k] != b)
              d = face[k];
          }
        }
      }

      // This edge does not have two faces.
      if (d == -1) {
        is_manifold_edge = false;
        break;
      }

      vec4 edge_point = (3.0f / 8.0f) * vertices[a] +
                        (3.0f / 8.0f) * vertices[b] +
                        (1.0f / 8.0f) * vertices[c] +
                        (1.0f / 8.0f) * vertices[d];

      v.push_back(edge_point);
    }

    // Forget the edges on the side of the mesh.
    if (!is_manifold_edge)
      continue;

    // One triangle gets split into 4.
    f.push_back(uvec3(offset + 0, offset + 1, offset + 2));
    f.push_back(uvec3(offset + 0, faces[i][1], offset + 1));
    f.push_back(uvec3(offset + 1, faces[i][2], offset + 2));
    f.push_back(uvec3(offset + 2, faces[i][0], offset + 0));
  }

  for (int i = 0; i < vertices.size(); i++) {
    set<int> unique_neighbors;
    for (int u : vertexFaces[i]) {
      for (int j = 0; j < 3; j++) {
        int v = faces[u][j];
        if (unique_neighbors.find(v) != unique_neighbors.end())
          continue;
        unique_neighbors.insert(v);
      }
    }

    // Take linear combination of neighbors.
    int n = unique_neighbors.size();
    float beta = 0.0f;
    if (n == 3)
      beta = 3.0f / 16.0f;
    else
      beta = 3.0f / (8.0f * n);

    vec4 new_position = (1.0f - n * beta) * vertices[i];
    for (int u : unique_neighbors)
      new_position += beta * vertices[u];

    v[i] = new_position;
  }

  // TODO: This one sucks.
  fixDuplicateVertices(v, f);
}

int main (int argc, char* argv[]) {
  LoadOBJ("./obj/cube.obj", cube_vertices, cube_faces, cube_normals);

  initOpenGL();

  camera_distance = 1.0f;
  eye = glm::vec3(1.0f, 1.0f, 1.0f);
  showWire = false;

  phongP.setup();
  floorP.setup();
  shadowP.setup();
  lineP.setup();
  wireP.setup();

  vec3 normal = normalize(vec3(0.0, 1.0, 0.0));
  planes.push_back(Plane(vec3(0.0, kFloorY-8*1e-2, 0.0), normal, 10.0f, 10.0f));

  objects.push_back(Geometry(cube_vertices, cube_faces));

  while (keepLoopingOpenGL()) {
    lineP.drawAxis();

    if (do_action) {
      Geometry& to_smooth = objects.back();

      vector<vec4> tmp_v;
      vector<uvec3> tmp_f;
      loop_subdivide(to_smooth.vertices, to_smooth.faces, tmp_v, tmp_f);

      objects.push_back(Geometry(tmp_v, tmp_f));
      objects.back().toWorld = to_smooth.toWorld * translate(vec3(1.5f, 0.0f, 0.0f));
      do_action = false;
    }

    for (Geometry& geom : objects) {
      if (showWire) {
        wireP.draw(geom.vertices, geom.faces, geom.toWorld, BLUE);
      }
      else {
        phongP.draw(geom.vertices, geom.faces, geom.normals,
                    geom.toWorld, GREEN, vec4(eye, 1.0f));
      }
    }

    for (Plane& plane: planes) {
      phongP.draw(plane.vertices, plane.faces, plane.normals, I,
                  BROWN, vec4(eye, 1.0f));
    }

    endLoopOpenGL();
  }

  cleanupOpenGL();
}
