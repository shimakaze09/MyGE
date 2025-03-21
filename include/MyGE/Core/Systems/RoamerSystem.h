#pragma once

#include <MyECS/World.h>

namespace My::MyGE {
struct RoamerSystem {
  static constexpr char SystemFuncName[] = "RoamerSystem";

  static void OnUpdate(MyECS::Schedule& schedule);
};
}  // namespace My::MyGE
