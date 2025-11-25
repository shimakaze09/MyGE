#pragma once

#include <MyECS/MyECS.hpp>

namespace My::MyGE {
struct LoggerSystem {
  static void OnUpdate(MyECS::Schedule&);
};
}  // namespace My::MyGE

