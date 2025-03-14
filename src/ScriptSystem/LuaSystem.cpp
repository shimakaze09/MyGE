//
// Created by Admin on 14/03/2025.
//

#include "LuaSystem.h"

#include <MyGE/ScriptSystem/LuaMngr.h>

using namespace My::MyGE;

void LuaSystem::Register(MyECS::World* world, std::string name,
                         sol::function onUpdate) {
  world->systemMngr.Register(
      std::unique_ptr<LuaSystem>(new LuaSystem(world, name, onUpdate)));
}

void LuaSystem::RegisterChunkFunc(MyECS::Schedule* s, sol::function onUpdate,
                                  std::string name,
                                  MyECS::EntityFilter filter) {
  s->Register(WrapChunkFunc(std::move(onUpdate)), std::move(name),
              std::move(filter));
}

std::function<void(My::MyECS::ChunkView)> LuaSystem::WrapChunkFunc(
    sol::function func) {
  auto bytes = func.dump();
  return [=](My::MyECS::ChunkView chunk) {
    auto L = LuaMngr::Instance().Request();
    {
      sol::state_view lua(L);
      sol::function f = lua.load(bytes.as_string_view());
      f.call(chunk);
    }
    LuaMngr::Instance().Recycle(L);
  };
}

LuaSystem::LuaSystem(MyECS::World* world, std::string name,
                     sol::function onUpdate)
    : MyECS::System{world, name}, onUpdate{onUpdate.dump()} {}

void LuaSystem::OnUpdate(MyECS::Schedule& schedule) {
  auto L = LuaMngr::Instance().Request();
  {
    sol::state_view lua(L);
    sol::function f = lua.load(onUpdate.as_string_view());
    f.call(&schedule);
  }
  LuaMngr::Instance().Recycle(L);
}
