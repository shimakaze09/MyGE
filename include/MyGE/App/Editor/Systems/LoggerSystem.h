#pragma once

#include <MyECS/MyECS.hpp>

namespace Smkz::MyGE {
struct LoggerSystem {
  static void OnUpdate(MyECS::Schedule&);
};
}  // namespace Smkz::MyGE
