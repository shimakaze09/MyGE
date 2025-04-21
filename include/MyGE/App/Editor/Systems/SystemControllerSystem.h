#pragma once

#include <MyECS/MyECS.hpp>

namespace Smkz::MyGE {
struct SystemControllerSystem {
  static void OnUpdate(MyECS::Schedule&);
};
}  // namespace Smkz::MyGE
