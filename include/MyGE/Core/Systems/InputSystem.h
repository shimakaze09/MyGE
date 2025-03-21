#pragma once

#include <MyECS/World.h>

namespace My::MyGE {
struct InputSystem {
  static constexpr char SystemFuncName[] = "InputSystem";

  static void OnUpdate(MyECS::Schedule& schedule);
};
}  // namespace My::MyGE
