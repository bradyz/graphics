#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_set>

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
#include "Geometry.h"
#include "octree.h"

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

vector<vec4> sphere_vertices;
vector<uvec3> sphere_faces;
vector<vec4> sphere_normals;

vec4 barycenter (const vec4& a, const vec4& b, const vec4& c) {
  vec4 bary(0.0f, 0.0f, 0.0f, 0.0f);
  bary += a;
  bary += b;
  bary += c;
  return bary / 3.0f;
}

void subdivide (vector<vec4>& vertices, vector<uvec3>& faces, 
                vector<vec4>& v, vector<uvec3>& f) {
  vector<vec4> verticesWithDuplicates;
  vector<uvec3> facesWithDuplicates;

  verticesWithDuplicates = vertices;

  map<int, vector<int> > vertexFaces;

  for (int i = 0; i < faces.size(); ++i) {
    for (int j = 0; j < 3; ++j)
      vertexFaces[faces[i][j]].push_back(i);
  }

  for (uvec3 face : faces) {
    vec4 barycenter;

    for (int i = 0; i < 3; ++i)
      barycenter += vertices[face[i]] / 3.0f;

    int barycenter_idx = verticesWithDuplicates.size();
    verticesWithDuplicates.push_back(barycenter);

    for (int i = 0; i < 3; ++i) {
      int a = face[i];
      int b = face[(i+1) % 3];

      vec4 edge_avg = (vertices[a] + vertices[b]) / 2.0f;
      int edge_avg_idx = verticesWithDuplicates.size();
      verticesWithDuplicates.push_back(edge_avg); 

      uvec3 new_face;
      new_face[0] = edge_avg_idx;
      new_face[1] = barycenter_idx;
      new_face[2] = a;

      facesWithDuplicates.push_back(new_face);
    }
  }

  cout << "with dupe: " << verticesWithDuplicates.size() << " " << facesWithDuplicates.size() << endl;

  fixDuplicateVertices(verticesWithDuplicates, facesWithDuplicates, v, f);

  cout << "without dupe: " << v.size() << " " << f.size() << endl;
}

void catmull (vector<vec4>& vertices, vector<uvec3>& faces, 
              vector<vec4>& v, vector<uvec3>& f) {
  v.clear();
  f.clear();

  map<int, vector<int> > vertexFaces;
  vector<vec4> barycenters;

  for (int i = 0; i < faces.size(); ++i) {
    const uvec3& face = faces[i];
    const vec4& a = vertices[face[0]];
    const vec4& b = vertices[face[1]];
    const vec4& c = vertices[face[2]];

    barycenters.push_back(barycenter(a, b, c));

    for (int j = 0; j < 3; ++j)
      vertexFaces[face[j]].push_back(i);
  }

  for (int i = 0; i < faces.size(); ++i) {
    vec4 edge_avg;
    set<int> neighbor_faces;

    for (int j = 0; j < 3; ++j) {
      int a = faces[i][j];
      int b = faces[i][(j+1) % 3];
      int neighbor = -1; 

      // find the other triangle that shares this edge
      for (int neighbor_idx : vertexFaces[faces[i][j]]) {
        neighbor_faces.insert(neighbor_idx);

        const uvec3& face = faces[neighbor_idx];

        bool a_found = false;
        bool b_found = false;

        for (int k = 0; k < 3; ++k) {
          a_found |= (a == face[k]);
          b_found |= (b == face[k]);
        }

        if (a_found && b_found) {
          neighbor = neighbor_idx;   
        }
      }

      assert(neighbor != -1);

      edge_avg += vertices[a];
      edge_avg += vertices[b];
      edge_avg += barycenters[i];
      edge_avg += barycenters[neighbor];
    } 

    edge_avg /= 12.0f;

    vec4 neighbor_avg;
    for (int neighbor_idx : neighbor_faces)
      neighbor_avg += barycenters[neighbor_idx];
    neighbor_avg /= static_cast<float>(neighbor_faces.size());

    vec4 new_vert = neighbor_avg + edge_avg * 2.0f;

    v.push_back(new_vert / 3.0f);
  }

}

int main (int argc, char* argv[]) {
  LoadOBJ("./obj/sphere.obj", sphere_vertices, sphere_faces, sphere_normals);
  fixSphereVertices(sphere_vertices);

  initOpenGL();

  camera_distance = 1.0f;
  eye = glm::vec3(1.0f, 1.0f, 1.0f);
  showWire = true;

  phongP.setup();
  floorP.setup();
  shadowP.setup();
  lineP.setup();
  wireP.setup();

  vec3 normal = normalize(vec3(0.0, 1.0, 0.0));
  planes.push_back(Plane(vec3(0.0, kFloorY-8*1e-2, 0.0), normal, 10.0f, 10.0f));
  
  objects.push_back(Geometry(sphere_vertices, sphere_faces));
  objects.back().normals = getVertexNormals(sphere_vertices, sphere_faces);

  mat4 D = translate(vec3(2.0f, 0.0f, 0.0f));

  vector<vec4> subvided_vertices;
  vector<uvec3> subvided_faces;

  subdivide(sphere_vertices, sphere_faces, subvided_vertices, subvided_faces);

  Geometry subdivided(subvided_vertices, subvided_faces);
  subdivided.toWorld = D;
  subdivided.normals = getVertexNormals(subvided_vertices, subvided_faces);

  objects.push_back(subdivided);

  vector<vec4> subdivided_vertices1;
  vector<uvec3> subdivided_faces1;

  subdivide(subvided_vertices, subvided_faces, subdivided_vertices1, subdivided_faces1);

  Geometry subdivided1(subdivided_vertices1, subdivided_faces1);
  subdivided1.toWorld = D * D;
  subdivided1.normals = getVertexNormals(subdivided_vertices1, subdivided_faces1);

  objects.push_back(subdivided1);

  while (keepLoopingOpenGL()) {
    lineP.drawAxis();

    for (Geometry& geom : objects) {
      if (showWire) {
        wireP.draw(geom.vertices, geom.faces, geom.toWorld, BLUE);
        for (vec4& vert : geom.vertices) {
          mat4 T = translate(vec3(vert)) * scale(vec3(0.03f, 0.03f, 0.03f));
          phongP.draw(sphere_vertices, sphere_faces, sphere_normals,
                      geom.toWorld * T, RED, vec4(eye, 1.0f));
        }
      }
      else {
        phongP.draw(geom.vertices, geom.faces, geom.normals,
                    geom.toWorld, GREEN, vec4(eye, 1.0f));
        shadowP.draw(geom.vertices, geom.faces, geom.toWorld);
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
