//
// Created by Admin on 20/03/2025.
//

#include <MyGE/ScriptSystem/LuaContext.h>
#include <MyGE/ScriptSystem/LuaCtxMngr.h>

#include <cassert>
#include <map>
#include <memory>

using namespace My::MyGE;

struct LuaCtxMngr::Impl {
  std::map<const MyECS::World*, std::unique_ptr<LuaContext>> world2ctx;
};

LuaCtxMngr::LuaCtxMngr() : pImpl{new Impl} {}

LuaCtxMngr::~LuaCtxMngr() {
  delete pImpl;
}

LuaContext* LuaCtxMngr::Register(const MyECS::World* world) {
  auto target = pImpl->world2ctx.find(world);
  if (target != pImpl->world2ctx.end())
    return target->second.get();

  auto ctx = new LuaContext;
  pImpl->world2ctx.emplace_hint(target, world,
                                std::unique_ptr<LuaContext>{ctx});
  return ctx;
}

void LuaCtxMngr::Unregister(const MyECS::World* world) {
  pImpl->world2ctx.erase(world);
}

// if not registered, return nullptr
LuaContext* LuaCtxMngr::GetContext(const MyECS::World* world) {
  auto target = pImpl->world2ctx.find(world);
  if (target == pImpl->world2ctx.end())
    return nullptr;

  return target->second.get();
}

void LuaCtxMngr::Clear() {
  pImpl->world2ctx.clear();
}
