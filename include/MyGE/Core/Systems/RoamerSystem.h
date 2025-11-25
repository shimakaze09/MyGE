#pragma once

#include <MyECS/MyECS.hpp>

namespace My::MyGE {
struct RoamerSystem {
  static constexpr char SystemFuncName[] = "RoamerSystem";

  static void OnUpdate(MyECS::Schedule& schedule);
};
}  // namespace My::MyGE

