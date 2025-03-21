#pragma once

#include <MyECS/World.h>

namespace My::MyGE {
struct TRSToLocalToWorldSystem {
  static constexpr const char SystemFuncName[] = "TRSToWorldToLocalSystem";

  static void OnUpdate(MyECS::Schedule& schedule);
};
}  // namespace My::MyGE
