#include <utils/gl_utils.hpp>

namespace gl_utils
{
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

ShaderDefinition::ShaderDefinition(
    size_t id, GLenum shader_type, std::string full_path,
    std::vector<InterfaceToName> interface_to_names)
    : _id(id),
      _shader_type(shader_type),
      _full_path(full_path),
      _resources(std::move(interface_to_names))
{
  buildShaderProgram();
  buildResourceToLocationMap();
}

GLuint ShaderDefinition::buildShaderProgram()
{
  std::string shader_code;
  try
  {
    shader_code = io_utils::loadFile(_full_path);
  } catch (const std::system_error& e)
  {
    spdlog::critical("Failed to load shader {}", _full_path);
    exit(EXIT_FAILURE);
  }
  const GLchar* shader_code_ptr = shader_code.c_str();

  _program_id = glCreateShaderProgramv(GL_VERTEX_SHADER, 1, &shader_code_ptr);
  GLint status;
  glGetProgramiv(_program_id, GL_LINK_STATUS, &status);
  if (status == GL_FALSE)
  {
    spdlog::critical("Failed to link shader program\n{}",
                     getProgramInfoLog(_program_id));
    exit(EXIT_FAILURE);
  }
  return _program_id;
}

void ShaderDefinition::buildResourceToLocationMap()
{
  for (auto resource : _resources)
  {
    _resource_locations[resource.name] = glGetProgramResourceLocation(
        _program_id, resource.program_interface, resource.name);
  }
}
}  // namespace gl_utils