#include <string>
#include <iostream>
#include <iomanip> 
#include <fstream>
#include <sstream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace std;

namespace glm {
std::ostream& operator<<(std::ostream& os, const glm::vec2& v) {
  os << glm::to_string(v);
  return os;
}

std::ostream& operator<<(std::ostream& os, const glm::vec3& v) {
  os << glm::to_string(v);
  return os;
}

std::ostream& operator<<(std::ostream& os, const glm::vec4& v) {
  os << glm::to_string(v);
  return os;
}

std::ostream& operator<<(std::ostream& os, const glm::mat4& v) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j)
      os << std::setprecision(3) << v[j][i] << "\t";
    os << std::endl;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const glm::mat3& v) {
  os << glm::to_string(v);
  return os;
}
}  // namespace glm

void LoadOBJ(const string file, 
             vector<glm::vec4>& vertices,
             vector<glm::uvec3>& indices) {
  fstream ifs(file);
  string buffer;
  while (getline(ifs, buffer)) {
    stringstream ss(buffer);
    char type;
    ss >> type;
    if (type == 'f') {
      int i, j, k;
      ss >> i >> j >> k;
      indices.push_back(glm::uvec3(i-1, j-1, k-1));
    }
    else if (type == 'v') {
      float x, y, z;
      ss >> x >> y >> z;
      vertices.push_back(glm::vec4(x, y, z, 1.0));
    }
  }
}

void LoadOBJWithNormals(const string file, 
                        vector<glm::vec4>& vertices,
                        vector<glm::uvec3>& indices,
                        vector<glm::vec4>& normals) {
  ifstream ifs(file);
  string buffer;
  while (getline(ifs, buffer)) {
    string type;
    stringstream ss(buffer);
    ss >> type;
    if (type == "v") {
      double x, y, z;
      ss >> x >> y >> z;
      vertices.push_back(glm::vec4(x, y, z, 1.0));
    } 
    else if (type == "vn"){
      double x, y, z;
      ss >> x >> y >> z;
      normals.push_back(glm::vec4(x, y, z, 1.0));
    }
    else if (type == "f") {
      // of the form 22283//22283      
      glm::uvec3 idx;
      for (int i = 0; i < 3; ++i) {
        string tmp;
        ss >> tmp;
        for (int j = 0; j < tmp.size() && tmp[j] != '/'; ++j) {
          idx[i] = idx[i] * 10 + int(tmp[j]) - '0';
        }
        idx[i] -= 1;
      }
      indices.push_back(idx);
    }
  }
}

vector<glm::vec4> getVertexNormals (const vector<glm::vec4>& vertices,
                                    const vector<glm::uvec3>& faces) {
  vector<glm::vec4> normals(vertices.size());
  for (const glm::uvec3& face: faces) {
    int v1 = face[0];
    int v2 = face[1];
    int v3 = face[2];
    glm::vec3 a = glm::vec3(vertices[v1]);
    glm::vec3 b = glm::vec3(vertices[v2]);
    glm::vec3 c = glm::vec3(vertices[v3]);
    glm::vec3 u = glm::normalize(b - a);
    glm::vec3 v = glm::normalize(c - a);
    glm::vec4 n = glm::vec4(glm::normalize(glm::cross(u, v)), 0.0f);
    normals[v1] += n;
    normals[v2] += n;
    normals[v3] += n;
  }
  for (int i = 0; i < normals.size(); ++i)
    normals[i] = glm::normalize(normals[i]);
  return normals;
}

string loadShader (const string filename) {
  cout << filename << endl;
  ifstream file(filename);
  stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}
