#include "tred/render/camera.h"

#include <cmath>

namespace render
{

CameraVectors
Camera::create_vectors() const
{
    const auto direction = glm::vec3
    {
        std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch)),
        std::sin(glm::radians(pitch)),
        std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch))
    };
    const auto front = glm::normalize(direction);
    const auto right = glm::normalize(glm::cross(front, UP));
    const auto up = glm::normalize(glm::cross(right, front));

    return {front, right, up, position};
}

}
