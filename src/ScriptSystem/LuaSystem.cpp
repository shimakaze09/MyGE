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

// World, Entity, size_t index, CmptsView
const My::MyECS::SystemFunc* LuaSystem::RegisterSystemFunc_Entity(
    MyECS::Schedule* s, sol::function systemFunc, std::string name,
    MyECS::EntityLocator locator, MyECS::EntityFilter filter) {
  assert(!locator.CmptTypes().empty());
  auto bytes = systemFunc.dump();
  auto sysfunc = s->Register(
      [bytes, locator = std::move(locator)](MyECS::World* w,
                                            MyECS::ChunkView chunk) {
        if (chunk.EntityNum() == 0) return;

        auto L = LuaMngr::Instance().Request();
        {
          sol::state_view lua(L);
          sol::function f = lua.load(bytes.as_string_view());
          auto arrayEntity = chunk.GetCmptArray<MyECS::Entity>();
          std::vector<void*> cmpts;
          std::vector<size_t> sizes;
          cmpts.reserve(locator.CmptTypes().size());
          sizes.reserve(locator.CmptTypes().size());
          for (const auto& t : locator.CmptTypes()) {
            cmpts.push_back(chunk.GetCmptArray(t));
            sizes.push_back(MyECS::RTDCmptTraits::Instance().Sizeof(t));
          }

          size_t i = 0;
          do {
            MyECS::CmptsView view{&locator, cmpts.data()};
            f(w, arrayEntity[i], i, view);
            for (size_t j = 0; j < cmpts.size(); j++)
              cmpts[j] = (reinterpret_cast<uint8_t*>(cmpts[j]) + sizes[j]);
          } while (++i < chunk.EntityNum());
        }
        LuaMngr::Instance().Recycle(L);
      },
      std::move(name), std::move(filter));
  return sysfunc;
}

// World, ChunkView
const My::MyECS::SystemFunc* LuaSystem::RegisterSystemFunc_Chunk(
    MyECS::Schedule* s, sol::function systemFunc, std::string name,
    MyECS::EntityFilter filter) {
  assert(!filter.AllCmptTypes().empty() || !filter.AnyCmptTypes().empty());
  auto bytes = systemFunc.dump();
  auto sysfunc = s->Register(
      [bytes](MyECS::World* w, MyECS::ChunkView chunk) {
        if (chunk.EntityNum() == 0) return;

        auto L = LuaMngr::Instance().Request();
        {
          sol::state_view lua(L);
          sol::function f = lua.load(bytes.as_string_view());
          f.call(w, chunk);
        }
        LuaMngr::Instance().Recycle(L);
      },
      std::move(name), std::move(filter));
  return sysfunc;
}

// World
const My::MyECS::SystemFunc* LuaSystem::RegisterSystemFunc_Job(
    MyECS::Schedule* s, sol::function systemFunc, std::string name) {
  auto bytes = systemFunc.dump();
  auto sysfunc = s->Register(
      [bytes](MyECS::World* w) {
        auto L = LuaMngr::Instance().Request();
        {
          sol::state_view lua(L);
          sol::function f = lua.load(bytes.as_string_view());
          f.call(w);
        }
        LuaMngr::Instance().Recycle(L);
      },
      std::move(name));
  return sysfunc;
}

LuaSystem::LuaSystem(MyECS::World* world, std::string name,
                     sol::function onUpdate)
    : MyECS::System{world, name}, onUpdate{onUpdate.dump()} {}

void LuaSystem::OnUpdate(MyECS::Schedule& schedule) {
  sol::state_view lua(LuaMngr::Instance().Main());
  sol::function f = lua.load(onUpdate.as_string_view());
  f.call(&schedule);
}
