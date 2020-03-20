#include <utils/gl_utils.hpp>
#include <utils/io_utils.hpp>

namespace gl_utils
{
ShaderProgram::ShaderProgram(size_t id, GLenum shader_type,
                             std::string full_path)
    : _id(id), _shader_type(shader_type), _full_path(full_path)
{
  buildShaderProgram();

  buildUniformMap();
  buildProgramInputMap();
  buildUniformBlockMap();
  buildShaderStorageBlockMap();
}

GLuint ShaderProgram::buildShaderProgram()
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

  _program_id = glCreateShaderProgramv(_shader_type, 1, &shader_code_ptr);
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

void ShaderProgram::buildUniformMap()
{
  glGetProgramInterfaceiv(_program_id, GL_UNIFORM, GL_ACTIVE_RESOURCES,
                          &_uniform_count);

  const GLenum props[] = {GL_NAME_LENGTH,
                          GL_LOCATION,
                          GL_TYPE,
                          GL_ARRAY_SIZE,
                          GL_OFFSET,
                          GL_BLOCK_INDEX,
                          GL_ARRAY_STRIDE,
                          GL_MATRIX_STRIDE,
                          GL_IS_ROW_MAJOR,
                          GL_ATOMIC_COUNTER_BUFFER_INDEX,
                          GL_REFERENCED_BY_VERTEX_SHADER,
                          GL_REFERENCED_BY_COMPUTE_SHADER,
                          GL_REFERENCED_BY_FRAGMENT_SHADER,
                          GL_REFERENCED_BY_GEOMETRY_SHADER,
                          GL_REFERENCED_BY_MESH_SHADER_NV,
                          GL_REFERENCED_BY_TASK_SHADER_NV,
                          GL_REFERENCED_BY_TESS_CONTROL_SHADER,
                          GL_REFERENCED_BY_TESS_EVALUATION_SHADER};

  constexpr GLsizei prop_count = std::size(props);

  for (int i = 0; i < _uniform_count; ++i)
  {
    GLint params[prop_count];
    glGetProgramResourceiv(_program_id, GL_UNIFORM, i, prop_count, props,
                           prop_count, nullptr, params);

    std::vector<char> name_buffer;
    name_buffer.resize(params[0]);
    glGetProgramResourceName(_program_id, GL_UNIFORM, i, name_buffer.size(),
                             nullptr, name_buffer.data());
    std::string name = {name_buffer.begin(), name_buffer.end() - 1};

    Uniform uniform{.name = name,
                    .location = params[1],
                    .type = params[2],
                    .array_size = params[3],
                    .offset = params[4],
                    .block_index = params[5],
                    .array_stride = params[6],
                    .matrix_stride = params[7],
                    .is_row_major = params[8],
                    .atomic_counter_buffer_index = params[9],
                    .referenced_by_vertex_shader = params[10],
                    .referenced_by_compute_shader = params[11],
                    .referenced_by_fragment_shader = params[12],
                    .referenced_by_geometry_shader = params[13],
                    .referenced_by_mesh_shader_nv = params[14],
                    .referenced_by_task_shader_nv = params[15],
                    .referenced_by_tess_control_shader = params[16],
                    .referenced_by_tess_evaluation_shader = params[17]};

    _uniforms.insert(std::make_pair(name, uniform));
  }
}

void ShaderProgram::buildProgramInputMap()
{
  glGetProgramInterfaceiv(_program_id, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES,
                          &_program_input_count);

  const GLenum props[] = {GL_NAME_LENGTH,
                          GL_LOCATION,
                          GL_TYPE,
                          GL_ARRAY_SIZE,
                          GL_REFERENCED_BY_VERTEX_SHADER,
                          GL_REFERENCED_BY_COMPUTE_SHADER,
                          GL_REFERENCED_BY_FRAGMENT_SHADER,
                          GL_REFERENCED_BY_GEOMETRY_SHADER,
                          GL_REFERENCED_BY_MESH_SHADER_NV,
                          GL_REFERENCED_BY_TASK_SHADER_NV,
                          GL_REFERENCED_BY_TESS_CONTROL_SHADER,
                          GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
                          GL_IS_PER_PATCH,
                          GL_LOCATION_COMPONENT};

  constexpr GLsizei prop_count = std::size(props);

  for (int i = 0; i < _program_input_count; ++i)
  {
    GLint params[prop_count];
    glGetProgramResourceiv(_program_id, GL_PROGRAM_INPUT, i, prop_count, props,
                           prop_count, nullptr, params);

    std::vector<char> name_buffer;
    name_buffer.resize(params[0]);
    glGetProgramResourceName(_program_id, GL_PROGRAM_INPUT, i,
                             name_buffer.size(), nullptr, name_buffer.data());
    std::string name = {name_buffer.begin(), name_buffer.end() - 1};

    ProgramInput pi{.name = name,
                    .location = params[1],
                    .type = params[2],
                    .array_size = params[3],
                    .referenced_by_vertex_shader = params[4],
                    .referenced_by_compute_shader = params[5],
                    .referenced_by_fragment_shader = params[6],
                    .referenced_by_geometry_shader = params[7],
                    .referenced_by_mesh_shader_nv = params[8],
                    .referenced_by_task_shader_nv = params[9],
                    .referenced_by_tess_control_shader = params[10],
                    .referenced_by_tess_evaluation_shader = params[11],
                    .is_per_patch = params[12],
                    .location_component = params[13]};

    _program_inputs.insert(std::make_pair(name, pi));
  }
}

void ShaderProgram::buildUniformBlockMap()
{
  glGetProgramInterfaceiv(_program_id, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES,
                          &_uniform_block_count);

  const GLenum props[] = {
      GL_NAME_LENGTH, GL_BUFFER_BINDING, GL_BUFFER_DATA_SIZE,
      // GL_NUM_ACTIVE_VARIABLES, GL_ACTIVE_VARIABLES,
      GL_REFERENCED_BY_VERTEX_SHADER, GL_REFERENCED_BY_COMPUTE_SHADER,
      GL_REFERENCED_BY_FRAGMENT_SHADER, GL_REFERENCED_BY_GEOMETRY_SHADER,
      GL_REFERENCED_BY_MESH_SHADER_NV, GL_REFERENCED_BY_TASK_SHADER_NV,
      GL_REFERENCED_BY_TESS_CONTROL_SHADER,
      GL_REFERENCED_BY_TESS_EVALUATION_SHADER};

  constexpr GLsizei prop_count = std::size(props);

  for (int i = 0; i < _uniform_block_count; ++i)
  {
    GLint params[prop_count];
    glGetProgramResourceiv(_program_id, GL_UNIFORM_BLOCK, i, prop_count, props,
                           prop_count, nullptr, params);

    std::vector<char> name_buffer;
    name_buffer.resize(params[0]);
    glGetProgramResourceName(_program_id, GL_UNIFORM_BLOCK, i,
                             name_buffer.size(), nullptr, name_buffer.data());
    std::string name = {name_buffer.begin(), name_buffer.end() - 1};

    UniformBlock ub{.name = name,
                    .buffer_binding = params[1],
                    .buffer_data_size = params[2],
                    .referenced_by_vertex_shader = params[3],
                    .referenced_by_compute_shader = params[4],
                    .referenced_by_fragment_shader = params[5],
                    .referenced_by_geometry_shader = params[6],
                    .referenced_by_mesh_shader_nv = params[7],
                    .referenced_by_task_shader_nv = params[8],
                    .referenced_by_tess_control_shader = params[9],
                    .referenced_by_tess_evaluation_shader = params[10]};

    _uniform_blocks.insert(std::make_pair(name, ub));
  }
}
void ShaderProgram::buildShaderStorageBlockMap()
{
  glGetProgramInterfaceiv(_program_id, GL_SHADER_STORAGE_BLOCK,
                          GL_ACTIVE_RESOURCES, &_shader_storage_block_count);

  const GLenum props[] = {
      GL_NAME_LENGTH, GL_BUFFER_BINDING, GL_BUFFER_DATA_SIZE,
      // GL_NUM_ACTIVE_VARIABLES, GL_ACTIVE_VARIABLES,
      GL_REFERENCED_BY_VERTEX_SHADER, GL_REFERENCED_BY_COMPUTE_SHADER,
      GL_REFERENCED_BY_FRAGMENT_SHADER, GL_REFERENCED_BY_GEOMETRY_SHADER,
      GL_REFERENCED_BY_MESH_SHADER_NV, GL_REFERENCED_BY_TASK_SHADER_NV,
      GL_REFERENCED_BY_TESS_CONTROL_SHADER,
      GL_REFERENCED_BY_TESS_EVALUATION_SHADER};

  constexpr GLsizei prop_count = std::size(props);

  for (int i = 0; i < _shader_storage_block_count; ++i)
  {
    GLint params[prop_count];
    glGetProgramResourceiv(_program_id, GL_SHADER_STORAGE_BLOCK, i, prop_count,
                           props, prop_count, nullptr, params);

    std::vector<char> name_buffer;
    name_buffer.resize(params[0]);
    glGetProgramResourceName(_program_id, GL_SHADER_STORAGE_BLOCK, i,
                             name_buffer.size(), nullptr, name_buffer.data());
    std::string name = {name_buffer.begin(), name_buffer.end() - 1};

    ShaderStorageBlock ssb{.name = name,
                           .buffer_binding = params[1],
                           .buffer_data_size = params[2],
                           .referenced_by_vertex_shader = params[3],
                           .referenced_by_compute_shader = params[4],
                           .referenced_by_fragment_shader = params[5],
                           .referenced_by_geometry_shader = params[6],
                           .referenced_by_mesh_shader_nv = params[7],
                           .referenced_by_task_shader_nv = params[8],
                           .referenced_by_tess_control_shader = params[9],
                           .referenced_by_tess_evaluation_shader = params[10]};

    _shader_storage_blocks.insert(std::make_pair(name, ssb));
  }
}

std::string ShaderProgram::getProgramInfoLog(GLuint program)
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

const std::string& ParameterQueries::getParameterValues(GLenum pname)
{
  if (!_parameter_cache_map.contains(pname))
  {
    GLfloat data;
    glGetFloatv(pname, &data);
    _parameter_cache_map[pname] = std::to_string(data);
  }
  return _parameter_cache_map[pname];
}

std::string ParameterQueries::getParameterValues(GLenum pname, GLuint index)
{
  GLfloat data;
  glGetFloati_v(pname, index, &data);

  return std::to_string(data);
}

const std::string& ParameterQueries::operator[](GLenum pname)
{
  return getParameterValues(pname);
}
}  // namespace gl_utils