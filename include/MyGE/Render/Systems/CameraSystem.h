#pragma once

#include <MyECS/MyECS.hpp>

namespace My::MyGE {
struct CameraSystem {
  static constexpr char SystemFuncName[] = "CameraSystem";

  static void OnUpdate(MyECS::Schedule& schedule);
};
}  // namespace My::MyGE

