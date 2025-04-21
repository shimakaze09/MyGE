#pragma once

#include <MyECS/MyECS.hpp>

namespace Smkz::MyGE {
struct TRSToLocalToParentSystem {
  static constexpr char SystemFuncName[] = "TRSToLocalToParentSystem";

  static void OnUpdate(MyECS::Schedule& schedule);
};
}  // namespace Smkz::MyGE
