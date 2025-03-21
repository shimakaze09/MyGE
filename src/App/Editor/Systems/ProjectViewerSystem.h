#pragma once

#include <MyECS/World.h>

namespace My::MyGE {
struct ProjectViewerSystem {
  static void OnUpdate(MyECS::Schedule&);
};
}  // namespace My::MyGE
