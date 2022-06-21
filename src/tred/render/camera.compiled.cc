#include "tred/render/camera.compiled.h"

#include "tred/render/camera.h"

namespace render
{

CompiledCamera::CompiledCamera(const glm::mat4& v, const glm::vec3& p)
    : view(v)
    , position(p)
{
}


CompiledCamera compile_camera(const CameraVectors& camera)
{
    const auto view = glm::lookAt(camera.position, camera.position + camera.front, UP);
    return {view, camera.position};
}

}
