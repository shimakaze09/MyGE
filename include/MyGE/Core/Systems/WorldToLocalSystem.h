#pragma once

#include <MyECS/MyECS.hpp>

namespace Smkz::MyGE {
struct WorldToLocalSystem {
  static constexpr char SystemFuncName[] = "WorldToLocalSystem";

  static void OnUpdate(MyECS::Schedule& schedule);
};
}  // namespace Smkz::MyGE
