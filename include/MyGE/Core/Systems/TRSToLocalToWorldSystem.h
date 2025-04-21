#pragma once

#include <MyECS/MyECS.hpp>

namespace Smkz::MyGE {
struct TRSToLocalToWorldSystem {
  static constexpr const char SystemFuncName[] = "TRSToLocalToWorldSystem";

  static void OnUpdate(MyECS::Schedule& schedule);
};
}  // namespace Smkz::MyGE
