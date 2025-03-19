//
// Created by Admin on 19/03/2025.
//

#pragma once

#include <MyECS/World.h>

namespace My::MyGE {
struct Hierarchy {
  MyECS::World* world{nullptr};
  MyECS::Entity select;
  MyECS::Entity hover;
};
}  // namespace My::MyGE
