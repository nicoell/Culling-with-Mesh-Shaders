#include <utils/gl_utils.hpp>

namespace gl_utils
{


GLuint createShaderProgram (GLenum shader_type,
                                  const std::string& shader_path)
{
  std::string shader_code;
  try
  {
  shader_code = io_utils::loadFile(shader_path);
  } catch (const std::system_error& e)
  {
    spdlog::critical("Failed to load shader {}", shader_path);
    exit(EXIT_FAILURE); 
  }
  const GLchar* shader_code_ptr = shader_code.c_str();

  const auto shader_program =
      glCreateShaderProgramv(GL_VERTEX_SHADER, 1, &shader_code_ptr);
  GLint status;
  glGetProgramiv(shader_program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE)
  {
    spdlog::critical("Failed to link shader program\n{}",
                  getProgramInfoLog(shader_program));
    exit(EXIT_FAILURE);
  }
  return shader_program;
}

std::string getProgramInfoLog(GLuint program)
{
  GLint logLen;
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);

  std::string log;
  if (logLen > 0)
  {
    log.resize(logLen, ' ');
    GLsizei written;
    glGetProgramInfoLog(program, logLen, &written, &log[0]);
  }
  return log;
}
}  // namespace gl_utils