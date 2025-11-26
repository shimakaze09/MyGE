#pragma once

#include <MyECS/MyECS.hpp>
#include <set>
#include <string>

namespace My::MyGE {
struct Hierarchy {
  MyECS::World* world{nullptr};
  std::set<MyECS::Entity> selecties;
  MyECS::Entity hover{MyECS::Entity::Invalid()};

  bool is_saving_world{false};
  bool is_saving_entities{false};
  std::string saved_path;
};
}  // namespace My::MyGE
