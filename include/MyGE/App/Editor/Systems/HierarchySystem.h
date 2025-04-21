#pragma once

#include <MyECS/MyECS.hpp>

namespace Smkz::MyGE {
struct HierarchySystem {
  static void OnUpdate(MyECS::Schedule&);
};
}  // namespace Smkz::MyGE
