#include <utils/glm_utils.hpp>

std::vector<glm::vec4> glm_utils::getClippingPlanes(const glm::mat4 &matrix)
{
  auto planes = std::vector<glm::vec4>(6);

  // Left clipping plane
  planes[0].x = matrix[0][3] + matrix[0][0];
  planes[0].y = matrix[1][3] + matrix[1][0];
  planes[0].z = matrix[2][3] + matrix[2][0];
  planes[0].w = matrix[3][3] + matrix[3][0];
  // Right clipping plane
  planes[1].x = matrix[0][3] - matrix[0][0];
  planes[1].y = matrix[1][3] - matrix[1][0];
  planes[1].z = matrix[2][3] - matrix[2][0];
  planes[1].w = matrix[3][3] - matrix[3][0];
  // Top clipping plane
  planes[2].x = matrix[0][3] - matrix[0][1];
  planes[2].y = matrix[1][3] - matrix[1][1];
  planes[2].z = matrix[2][3] - matrix[2][1];
  planes[2].w = matrix[3][3] - matrix[3][1];
  // Bottom clipping plane
  planes[3].x = matrix[0][3] + matrix[0][1];
  planes[3].y = matrix[1][3] + matrix[1][1];
  planes[3].z = matrix[2][3] + matrix[2][1];
  planes[3].w = matrix[3][3] + matrix[3][1];
  // Near clipping plane
  planes[4].x = matrix[0][3] + matrix[0][2];
  planes[4].y = matrix[1][3] + matrix[1][2];
  planes[4].z = matrix[2][3] + matrix[2][2];
  planes[4].w = matrix[3][3] + matrix[3][2];
  // Far clipping plane
  planes[5].x = matrix[0][3] - matrix[0][2];
  planes[5].y = matrix[1][3] - matrix[1][2];
  planes[5].z = matrix[2][3] - matrix[2][2];
  planes[5].w = matrix[3][3] - matrix[3][2];

  for (int i = 0; i < 6; ++i)
  {
    const float inv_length =
        1.0f / glm::sqrt(planes[i].x * planes[i].x + planes[i].y * planes[i].y +
                         planes[i].z * planes[i].z);
    planes[i] *= inv_length;
  }
  return planes;
}
