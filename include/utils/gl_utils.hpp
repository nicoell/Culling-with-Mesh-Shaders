#pragma once
#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <map>
#include <string>

namespace gl_utils
{
struct Uniform
{
  std::string name;
  GLint location;
  GLint type;
  GLint array_size;
  GLint offset;
  GLint block_index;
  GLint array_stride;
  GLint matrix_stride;
  GLint is_row_major;
  GLint atomic_counter_buffer_index;
  GLint referenced_by_vertex_shader;
  GLint referenced_by_compute_shader;
  GLint referenced_by_fragment_shader;
  GLint referenced_by_geometry_shader;
  GLint referenced_by_mesh_shader_nv;
  GLint referenced_by_task_shader_nv;
  GLint referenced_by_tess_control_shader;
  GLint referenced_by_tess_evaluation_shader;
};

struct ProgramInput
{
  std::string name;
  GLint location;
  GLint type;
  GLint array_size;
  GLint referenced_by_vertex_shader;
  GLint referenced_by_compute_shader;
  GLint referenced_by_fragment_shader;
  GLint referenced_by_geometry_shader;
  GLint referenced_by_mesh_shader_nv;
  GLint referenced_by_task_shader_nv;
  GLint referenced_by_tess_control_shader;
  GLint referenced_by_tess_evaluation_shader;
  GLint is_per_patch;
  GLint location_component;
};

struct UniformBlock
{
  std::string name;
  GLint buffer_binding;
  GLint buffer_data_size;
  GLint num_active_variables;
  GLint active_variables;
  GLint referenced_by_vertex_shader;
  GLint referenced_by_compute_shader;
  GLint referenced_by_fragment_shader;
  GLint referenced_by_geometry_shader;
  GLint referenced_by_mesh_shader_nv;
  GLint referenced_by_task_shader_nv;
  GLint referenced_by_tess_control_shader;
  GLint referenced_by_tess_evaluation_shader;
};

struct ShaderStorageBlock
{
  std::string name;
  GLint buffer_binding;
  GLint buffer_data_size;
  GLint num_active_variables;
  GLint active_variables;
  GLint referenced_by_vertex_shader;
  GLint referenced_by_compute_shader;
  GLint referenced_by_fragment_shader;
  GLint referenced_by_geometry_shader;
  GLint referenced_by_mesh_shader_nv;
  GLint referenced_by_task_shader_nv;
  GLint referenced_by_tess_control_shader;
  GLint referenced_by_tess_evaluation_shader;
};

class ShaderProgram
{
 public:
  ShaderProgram(size_t id, GLenum shader_type, std::string full_path);

  explicit operator size_t() const { return _id; }
  explicit operator GLuint() const { return _program_id; }

  const Uniform& getUniform(const std::string& name) const
  {
    return _uniforms.at(name);
  }
  const UniformBlock& getUniformBlock(const std::string& name) const
  {
    return _uniform_blocks.at(name);
  }
  const ProgramInput& getProgramInput(const std::string& name) const
  {
    return _program_inputs.at(name);
  }
  const ShaderStorageBlock& getShaderStorageBlock(const std::string& name) const
  {
    return _shader_storage_blocks.at(name);
  }

 private:
  GLuint buildShaderProgram();
  void buildUniformMap();
  void buildProgramInputMap();
  void buildUniformBlockMap();
  void buildShaderStorageBlockMap();

  static std::string getProgramInfoLog(GLuint program);

  size_t _id;
  GLenum _shader_type;
  std::string _full_path;

  GLint _uniform_count{};
  std::map<std::string, Uniform> _uniforms;
  GLint _program_input_count{};
  std::map<std::string, ProgramInput> _program_inputs;
  GLint _uniform_block_count{};
  std::map<std::string, UniformBlock> _uniform_blocks;
  GLint _shader_storage_block_count{};
  std::map<std::string, ShaderStorageBlock> _shader_storage_blocks;

  GLuint _program_id{};
};

class ParameterQueries
{
 public:
  const std::string& getParameterValues(GLenum pname);
  std::string getParameterValues(GLenum pname, GLuint index);
  const std::string& operator[](GLenum pname);

 private:
  std::map<GLenum, std::string> _parameter_cache_map;
};

}  // namespace gl_utils
