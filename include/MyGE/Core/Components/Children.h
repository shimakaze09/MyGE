#pragma once

#include <MyECS/Entity.h>

#include <set>

namespace My::MyGE {
struct Children {
  std::set<MyECS::Entity> value;
};
}  // namespace My::MyGE

#include "details/Children_AutoRefl.inl"
