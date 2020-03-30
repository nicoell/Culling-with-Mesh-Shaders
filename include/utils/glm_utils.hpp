#pragma once
#include <glm/mat4x4.hpp>
#include <vector>

namespace glm_utils
{
std::vector<glm::vec4> getClippingPlanes(const glm::mat4 &matrix);

}  // namespace glm_utils
