#pragma once

#include <vector>

#include "glm/glm.hpp"


/** A unique combination of position/normal/texturecoord/etc. in a Mesh.
 */
struct vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture;
    glm::vec4 color;

    vertex
    (
        const glm::vec3& p,
        const glm::vec3& n,
        const glm::vec2& t,
        const glm::vec4& c = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}
    );
};


using triangle = glm::ivec3;


struct mesh
{
    std::vector<vertex> vertices;
    std::vector<unsigned int> triangles; // %3 == 0

    mesh(const std::vector<vertex>& verts, const std::vector<unsigned int>& tris);
};

