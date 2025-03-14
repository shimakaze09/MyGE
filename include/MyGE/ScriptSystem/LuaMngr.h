//
// Created by Admin on 14/03/2025.
//

#pragma once

#include <MyLuaPP/MyLuaPP.h>

#include <set>

namespace My::MyGE {
// World
// - GetSystemMngr
// - GetEntityMngr
class LuaMngr {
 public:
  static LuaMngr& Instance() {
    static LuaMngr instance;
    return instance;
  }

  void Init();

  void Reserve(size_t n);

  lua_State* Main() const;

  // lock
  lua_State* Request();
  // lock
  void Recycle(lua_State*);

  void Clear();

 private:
  struct Impl;
  Impl* pImpl{nullptr};
};
}  // namespace My::MyGE
