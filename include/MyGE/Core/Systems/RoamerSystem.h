#pragma once

#include <MyECS/MyECS.hpp>

namespace Smkz::MyGE {
struct RoamerSystem {
  static constexpr char SystemFuncName[] = "RoamerSystem";

  static void OnUpdate(MyECS::Schedule& schedule);
};
}  // namespace Smkz::MyGE
