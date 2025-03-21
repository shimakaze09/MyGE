//
// Created by Admin on 16/03/2025.
//

#pragma once

#include <MyECS/World.h>

namespace My::MyGE {
struct WorldTimeSystem {
  static constexpr char SystemFuncName[] = "WorldTimeSystem";

  static void OnUpdate(MyECS::Schedule& schedule);
};
}  // namespace My::MyGE
