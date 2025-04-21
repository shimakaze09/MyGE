#pragma once

#include <MyECS/MyECS.hpp>

namespace Smkz::MyGE {
struct InspectorSystem {
  static void OnUpdate(MyECS::Schedule&);
};
}  // namespace Smkz::MyGE
