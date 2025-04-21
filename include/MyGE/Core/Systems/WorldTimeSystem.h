#pragma once

#include <MyECS/MyECS.hpp>

namespace Smkz::MyGE {
struct WorldTimeSystem {
  static constexpr char SystemFuncName[] = "WorldTimeSystem";

  static void OnUpdate(MyECS::Schedule& schedule);
};
}  // namespace Smkz::MyGE
