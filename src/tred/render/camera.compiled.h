#pragma once

namespace render
{

struct CameraVectors;


struct CompiledCamera
{
    glm::mat4 view;
    glm::vec3 position;

    CompiledCamera(const glm::mat4& v, const glm::vec3& p);
};


CompiledCamera compile_camera(const CameraVectors& camera);

}
