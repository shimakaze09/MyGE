//
// Created by Admin on 13/03/2025.
//

#pragma once

#include <MyECS/World.h>

namespace My::MyGE {
struct TRSToLocalToWorldSystem {
  static constexpr const char SystemFuncName[] = "TRSToWorldToLocalSystem";

  static void OnUpdate(MyECS::Schedule& schedule);
};
}  // namespace My::MyGE
