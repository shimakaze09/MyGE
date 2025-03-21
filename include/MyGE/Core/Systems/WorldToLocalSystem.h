#pragma once

#include <MyECS/World.h>

namespace My::MyGE {
struct WorldToLocalSystem {
  static constexpr char SystemFuncName[] = "WorldToLocalSystem";

  static void OnUpdate(MyECS::Schedule& schedule);
};
}  // namespace My::MyGE
