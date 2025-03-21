#pragma once

#include <MyECS/World.h>

namespace My::MyGE {
struct Hierarchy {
  MyECS::World* world{nullptr};
  MyECS::Entity select;
  MyECS::Entity hover;
};
}  // namespace My::MyGE
