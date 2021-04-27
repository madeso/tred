#include "tred/mesh.h"


vertex::vertex
(
    const glm::vec3& p,
    const glm::vec3& n,
    const glm::vec2& t,
    const glm::vec4& c
)
    : position(p)
    , normal(n)
    , texture(t)
    , color(c)
{
}


mesh::mesh(const std::vector<vertex>& verts, const std::vector<unsigned int>& tris)
    : vertices(verts)
    , triangles(tris)
{
    assert(triangles.size() % 3 == 0);
}
