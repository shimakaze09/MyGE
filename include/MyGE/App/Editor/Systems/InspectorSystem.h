#pragma once

#include <MyECS/MyECS.hpp>

namespace My::MyGE {
struct InspectorSystem {
  static void OnUpdate(MyECS::Schedule&);
};
}  // namespace My::MyGE

