#pragma once

#include <MyECS/World.h>

namespace My::MyGE {
struct HierarchySystem {
  static void OnUpdate(MyECS::Schedule&);
};
}  // namespace My::MyGE
