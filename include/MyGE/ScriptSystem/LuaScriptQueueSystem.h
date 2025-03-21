#pragma once

#include <MyECS/World.h>

namespace My::MyGE {
struct LuaScriptQueueSystem {
  static void OnUpdate(MyECS::Schedule& schedule);
};
}  // namespace My::MyGE
