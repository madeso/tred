#include "tred/render/geom.h"

#include "tred/assert.h"

Vertex::Vertex
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


Geom::Geom(const std::vector<Vertex>& verts, const std::vector<unsigned int>& tris)
    : vertices(verts)
    , triangles(tris)
{
    ASSERT(triangles.size() % 3 == 0);
}
