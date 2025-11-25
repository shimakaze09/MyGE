#pragma once

#include <MyECS/MyECS.hpp>

namespace My::MyGE {
struct WorldTimeSystem {
  static constexpr char SystemFuncName[] = "WorldTimeSystem";

  static void OnUpdate(MyECS::Schedule& schedule);
};
}  // namespace My::MyGE

