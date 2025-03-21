#pragma once

#include <MyECS/World.h>

namespace My::MyGE {
struct TRSToLocalToParentSystem {
  static constexpr char SystemFuncName[] = "TRSToLocalToParentSystem";

  static void OnUpdate(MyECS::Schedule& schedule);
};
}  // namespace My::MyGE
