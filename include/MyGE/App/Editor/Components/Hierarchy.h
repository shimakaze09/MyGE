#pragma once

#include <MyECS/MyECS.hpp>

namespace Smkz::MyGE {
struct Hierarchy {
  MyECS::World* world{nullptr};
  MyECS::Entity select;
  MyECS::Entity hover;
};
}  // namespace Smkz::MyGE
