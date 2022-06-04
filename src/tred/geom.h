#pragma once






/** A unique combination of position/normal/texturecoord/etc. in a Geom.
 */
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture;
    glm::vec4 color;

    Vertex
    (
        const glm::vec3& p,
        const glm::vec3& n,
        const glm::vec2& t,
        const glm::vec4& c = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}
    );
};


using Triangle = glm::ivec3;


struct Geom
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> triangles; // %3 == 0

    Geom(const std::vector<Vertex>& verts, const std::vector<unsigned int>& tris);
};

