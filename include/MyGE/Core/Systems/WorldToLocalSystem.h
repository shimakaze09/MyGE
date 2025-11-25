#pragma once

#include <MyECS/MyECS.hpp>

namespace My::MyGE {
struct WorldToLocalSystem {
  static constexpr char SystemFuncName[] = "WorldToLocalSystem";

  static void OnUpdate(MyECS::Schedule& schedule);
};
}  // namespace My::MyGE

