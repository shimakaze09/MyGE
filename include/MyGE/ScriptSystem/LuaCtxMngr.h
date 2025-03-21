#pragma once

namespace My::MyECS {
class World;
}

namespace My::MyGE {
class LuaContext;

class LuaCtxMngr {
 public:
  static LuaCtxMngr& Instance() {
    static LuaCtxMngr instance;
    return instance;
  }

  LuaContext* Register(const MyECS::World* world);
  void Unregister(const MyECS::World* world);

  // if not registered, return nullptr
  LuaContext* GetContext(const MyECS::World* world);

  void Clear();

 private:
  LuaCtxMngr();
  ~LuaCtxMngr();
  struct Impl;
  Impl* pImpl{nullptr};
};
}  // namespace My::MyGE
