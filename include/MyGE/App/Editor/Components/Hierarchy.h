#pragma once

#include <MyECS/MyECS.hpp>
#include <string>

namespace My::MyGE {
struct Hierarchy {
  MyECS::World* world{nullptr};
  MyECS::Entity select{MyECS::Entity::Invalid()};
  MyECS::Entity hover{MyECS::Entity::Invalid()};

  bool is_saving_world{false};
  std::string saved_path;
};
}  // namespace My::MyGE

