#pragma once

#include <MyECS/MyECS.hpp>

namespace Smkz::MyGE {
struct InputSystem {
  static constexpr char SystemFuncName[] = "InputSystem";

  static void OnUpdate(MyECS::Schedule& schedule);
};
}  // namespace Smkz::MyGE
