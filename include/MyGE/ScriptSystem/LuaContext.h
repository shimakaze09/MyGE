//
// Created by Admin on 15/03/2025.
//

#pragma once

struct lua_State;

namespace My::MyECS {
class LuaContext {
 public:
  LuaContext();
  ~LuaContext();

  void Reserve(size_t n);

  lua_State* Main() const;

  // lock
  lua_State* Request();

  // lock
  void Recycle(lua_State*);

  // this won't clear main
  void Clear();

 private:
  struct Impl;
  Impl* pImpl;
};
}  // namespace My::MyECS
