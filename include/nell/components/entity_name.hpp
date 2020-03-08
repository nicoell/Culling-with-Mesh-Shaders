#pragma once

namespace nell::comp
{
struct EntityName
{
  EntityName(std::string name) : name(std::move(name)){};
  std::string name;
};

template <typename Archive>
void serialize(Archive &archive, EntityName &entity_name)
{
  archive(entity_name.name);
}

inline void drawComponentImpl(EntityName &entity_name) {}

}  // namespace nell::comp
