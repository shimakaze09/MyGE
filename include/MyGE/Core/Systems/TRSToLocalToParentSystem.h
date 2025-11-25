#pragma once

#include <MyECS/MyECS.hpp>

namespace My::MyGE {
struct TRSToLocalToParentSystem {
  static constexpr char SystemFuncName[] = "TRSToLocalToParentSystem";

  static void OnUpdate(MyECS::Schedule& schedule);
};
}  // namespace My::MyGE

