#pragma once

#include <MyECS/World.h>

namespace My::MyGE {
struct SystemControllerSystem {
  static void OnUpdate(MyECS::Schedule&);
};
}  // namespace My::MyGE