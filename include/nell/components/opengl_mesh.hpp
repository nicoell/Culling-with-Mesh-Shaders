#pragma once
#include <glad/glad.h>

namespace nell::comp
{
// TODO: Refactor this together with mesh struct.
struct BasicVertexArrayData
{
  GLuint vertex_array_object;  // TODO: Not needed here
  GLuint vertex_buffer_object;
  GLuint normal_buffer_object;
  GLuint index_buffer_object;
};

}  // namespace nell::comp
