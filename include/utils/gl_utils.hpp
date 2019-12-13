#pragma once
#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <map>
#include <string>
#include <utils/io_utils.hpp>

namespace gl_utils
{
std::string getProgramInfoLog(GLuint program);

struct InterfaceToName
{
  GLenum program_interface;
  const char* name;
};

class ShaderDefinition
{
 public:
  ShaderDefinition(
      size_t id, GLenum shader_type, std::string full_path,
      std::vector<InterfaceToName> interface_to_names);

  GLuint operator[](const char* c) { return _resource_locations[c]; }
  explicit operator size_t() const { return _id; }
  explicit operator GLuint() const { return _program_id; }

 private:
  GLuint buildShaderProgram();
  void buildResourceToLocationMap();

  size_t _id;
  GLenum _shader_type;
  std::string _full_path;
  std::map<const char*, GLuint> _resource_locations;
  std::vector<InterfaceToName> _resources;
  GLuint _program_id;
};



}  // namespace gl_utils
