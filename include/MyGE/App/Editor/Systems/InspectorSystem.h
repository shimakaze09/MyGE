#pragma once

#include <MyECS/World.h>

namespace My::MyGE {
struct InspectorSystem {
  static void OnUpdate(MyECS::Schedule&);
};
}  // namespace My::MyGE
