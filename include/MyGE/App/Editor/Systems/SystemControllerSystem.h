#pragma once

#include <MyECS/MyECS.hpp>

namespace My::MyGE {
struct SystemControllerSystem {
  static void OnUpdate(MyECS::Schedule&);
};
}  // namespace My::MyGE

