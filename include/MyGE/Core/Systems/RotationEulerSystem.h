#pragma once

#include <MyECS/MyECS.hpp>

namespace My::MyGE {
struct RotationEulerSystem {
  static constexpr char SystemFuncName[] = "RotationEulerSystem";

  static void OnUpdate(MyECS::Schedule& schedule);
};
}  // namespace My::MyGE
