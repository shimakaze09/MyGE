#pragma once

#include <MyECS/World.h>

namespace My::MyGE {
struct TRSToLocalToWorldSystem {
  static constexpr const char SystemFuncName[] = "TRSToLocalToWorldSystem";

  static void OnUpdate(MyECS::Schedule& schedule);
};
}  // namespace My::MyGE
