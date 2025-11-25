#pragma once

#include <MyECS/MyECS.hpp>

namespace My::MyGE {
struct InputSystem {
  static constexpr char SystemFuncName[] = "InputSystem";

  static void OnUpdate(MyECS::Schedule& schedule);
};
}  // namespace My::MyGE

