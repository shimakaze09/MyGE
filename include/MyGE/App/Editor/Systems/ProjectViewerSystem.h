#pragma once

#include <MyECS/MyECS.hpp>

namespace My::MyGE {
struct ProjectViewerSystem {
  static void OnUpdate(MyECS::Schedule&);
};
}  // namespace My::MyGE

