#pragma once

#include <MyECS/MyECS.hpp>

namespace Smkz::MyGE {
struct CameraSystem {
  static constexpr char SystemFuncName[] = "CameraSystem";

  static void OnUpdate(MyECS::Schedule& schedule);
};
}  // namespace Smkz::MyGE
