//
// Created by Admin on 16/03/2025.
//

#pragma once

#include <MyECS/World.h>

namespace My::MyGE {
struct CameraSystem {
  static constexpr char SystemFuncName[] = "CameraSystem";

  static void OnUpdate(MyECS::Schedule& schedule);
};
}  // namespace My::MyGE
