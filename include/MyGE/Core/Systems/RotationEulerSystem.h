#pragma once

#include <MyECS/World.h>

namespace My::MyGE {
struct RotationEulerSystem {
  static constexpr char SystemFuncName[] = "RotationEulerSystem";

  static void OnUpdate(MyECS::Schedule& schedule);
};
}  // namespace My::MyGE
