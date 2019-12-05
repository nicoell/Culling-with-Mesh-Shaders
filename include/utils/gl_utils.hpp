#pragma once
#include <string>
#include <utils/io_utils.hpp>
#include <glad/glad.h>
#include <spdlog/spdlog.h>

namespace gl_utils
{
struct ShaderCodeHelper
{
  GLchar* ptr;
  std::string path;
  std::string source;
};

GLuint createShaderProgram(GLenum shader_type, const std::string& shader_path);

std::string getProgramInfoLog(GLuint program);
}
