#pragma once

#include <MyECS/MyECS.hpp>

namespace Smkz::MyGE {
struct RotationEulerSystem {
  static constexpr char SystemFuncName[] = "RotationEulerSystem";

  static void OnUpdate(MyECS::Schedule& schedule);
};
}  // namespace Smkz::MyGE