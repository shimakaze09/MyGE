#pragma once

#include <MyECS/Entity.hpp>
#include <vector>

namespace Smkz::MyGE {
struct Parent {
  MyECS::Entity value{MyECS::Entity::Invalid()};
};
}  // namespace Smkz::MyGE
