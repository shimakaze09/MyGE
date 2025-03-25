#pragma once

#include <memory>
#include <vector>

#include "LuaScript.h"

namespace My::MyGE {
struct LuaScriptQueue {
  std::vector<std::shared_ptr<LuaScript>> value;
};
}  // namespace My::MyGE

#include "details/LuaScriptQueue_AutoRefl.h"
