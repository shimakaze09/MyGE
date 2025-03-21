#pragma once

#include <MyECS/Entity.h>

#include <vector>

namespace My::MyGE {
struct Parent {
  MyECS::Entity value{MyECS::Entity::Invalid()};
};
}  // namespace My::MyGE

#include "details/Parent_AutoRefl.inl"
