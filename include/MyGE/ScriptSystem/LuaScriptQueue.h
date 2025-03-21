//
// Created by Admin on 21/03/2025.
//

#pragma once

#include <vector>

#include "LuaScript.h"

namespace My::MyGE {
struct LuaScriptQueue {
  std::vector<const LuaScript*> value;
};
}  // namespace My::MyGE

#include "details/LuaScriptQueue_AutoRefl.h"
