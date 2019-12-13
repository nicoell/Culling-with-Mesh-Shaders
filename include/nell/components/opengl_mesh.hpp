#pragma once
#include <glad/glad.h>

namespace nell::comp
{

struct BasicVertexArrayData
{
  GLuint vertex_array_object;
  GLuint vertex_buffer_object;
  GLuint normal_buffer_object;
  GLuint index_buffer_object;
};

}
