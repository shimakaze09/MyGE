//
// Created by Admin on 13/03/2025.
//

#pragma once

#include <MyECS/Entity.h>

#include <vector>

namespace My::MyGE {
struct Children {
  std::vector<MyECS::Entity> value;
};
}  // namespace My::MyGE

#include "details/Children_AutoRefl.inl"
