#ifndef RANDOMUTILS_H
#define RANDOMUTILS_H

#include <string>
#include <vector>

#include <glm/glm.hpp>

void LoadOBJ(const std::string file, 
             std::vector<glm::vec4>& vertices,
             std::vector<glm::uvec3>& indices);

void LoadOBJWithNormals(const std::string file, 
                        std::vector<glm::vec4>& vertices,
                        std::vector<glm::uvec3>& indices,
                        std::vector<glm::vec4>& normals);

std::vector<glm::vec4> getVertexNormals (const std::vector<glm::vec4>& vertices,
                                         const std::vector<glm::uvec3>& faces);

const glm::vec4 GREEN = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
const glm::vec4 BLUE = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
const glm::vec4 WHITE = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
const glm::vec4 CYAN = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
const glm::vec4 RED = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

#endif
