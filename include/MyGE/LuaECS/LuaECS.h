//
// Created by Admin on 14/03/2025.
//

#pragma once

#include <MyLuaPP/MyLuaPP.h>

namespace My::MyGE {
// World
// - GetSystemMngr
// - GetEntityMngr
class LuaECS {
 public:
  static LuaECS& Instance() {
    static LuaECS instance;
    return instance;
  }

  void Init(lua_State* L);
};
}  // namespace My::MyGE
