#pragma once

namespace nell::comp
{
template <class T>
struct Mesh
{
  virtual ~Mesh() = default;
  static unsigned requestImportFlags() { return T::getImportFlags(); }
};

}  // namespace nell::comp
