#pragma once

#include <MyECS/World.h>

namespace My::MyGE {
struct LoggerSystem {
  static void OnUpdate(MyECS::Schedule&);
};
}  // namespace My::MyGE
