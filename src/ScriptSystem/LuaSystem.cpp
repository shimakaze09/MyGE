//
// Created by Admin on 14/03/2025.
//

#include "LuaSystem.h"

#include <MyGE/ScriptSystem/LuaMngr.h>

using namespace My::MyGE;

void LuaSystem::RegisterSystem(MyECS::World* world, std::string name,
                               sol::function onUpdate) {
  world->systemMngr.Register(
      std::unique_ptr<LuaSystem>(new LuaSystem(world, name, onUpdate)));
}

const My::MyECS::SystemFunc* LuaSystem::RegisterEntityJob(
    MyECS::Schedule* s, sol::function systemFunc, std::string name,
    MyECS::ArchetypeFilter filter, MyECS::CmptLocator cmptLocator,
    MyECS::SingletonLocator singletonLocator) {
  assert(!cmptLocator.CmptTypes().empty());
  auto bytes = systemFunc.dump();
  auto sysfunc = s->RegisterChunkJob(
      [bytes = std::move(bytes), cmptLocator = std::move(cmptLocator)](
          MyECS::World* w, MyECS::SingletonsView singletonsView,
          MyECS::ChunkView chunk) {
        if (chunk.EntityNum() == 0) return;

        auto L = LuaMngr::Instance().Request();
        {
          sol::state_view lua(L);
          sol::function f = lua.load(bytes.as_string_view());

          auto arrayEntity = chunk.GetCmptArray<MyECS::Entity>();
          std::vector<void*> cmpts;
          std::vector<MyECS::CmptType> types;
          std::vector<MyECS::CmptPtr> cmptPtrs;
          std::vector<size_t> sizes;
          cmpts.reserve(cmptLocator.CmptTypes().size());
          types.reserve(cmptLocator.CmptTypes().size());
          cmptPtrs.reserve(cmptLocator.CmptTypes().size());
          sizes.reserve(cmptLocator.CmptTypes().size());
          for (const auto& t : cmptLocator.CmptTypes()) {
            cmpts.push_back(chunk.GetCmptArray(t));
            types.push_back(t);
            cmptPtrs.emplace_back(t, cmpts.back());
            sizes.push_back(MyECS::RTDCmptTraits::Instance().Sizeof(t));
          }

          size_t i = 0;
          do {
            MyECS::CmptsView view{cmptPtrs.data(), cmptPtrs.size()};
            f(w, singletonsView, arrayEntity[i], i, view);
            for (size_t j = 0; j < cmpts.size(); j++) {
              cmpts[j] = (reinterpret_cast<uint8_t*>(cmpts[j]) + sizes[j]);
              cmptPtrs[j] = {types[j], cmpts[j]};
            }
          } while (++i < chunk.EntityNum());
        }
        LuaMngr::Instance().Recycle(L);
      },
      std::move(name), std::move(filter), std::move(singletonLocator));
  return sysfunc;
}

const My::MyECS::SystemFunc* LuaSystem::RegisterChunkJob(
    MyECS::Schedule* s, sol::function systemFunc, std::string name,
    MyECS::ArchetypeFilter filter, MyECS::SingletonLocator singletonLocator) {
  auto bytes = systemFunc.dump();
  auto sysfunc = s->RegisterChunkJob(
      [bytes](MyECS::World* w, MyECS::SingletonsView singletonsView,
              MyECS::ChunkView chunk) {
        auto L = LuaMngr::Instance().Request();
        {
          sol::state_view lua(L);
          sol::function f = lua.load(bytes.as_string_view());
          f.call(w, singletonsView, chunk);
        }
        LuaMngr::Instance().Recycle(L);
      },
      std::move(name), std::move(filter), std::move(singletonLocator));
  return sysfunc;
}

const My::MyECS::SystemFunc* LuaSystem::RegisterJob(
    MyECS::Schedule* s, sol::function systemFunc, std::string name,
    MyECS::SingletonLocator singletonLocator) {
  auto bytes = systemFunc.dump();
  auto sysfunc = s->RegisterJob(
      [bytes](MyECS::World* w, MyECS::SingletonsView singletonsView) {
        auto L = LuaMngr::Instance().Request();
        {
          sol::state_view lua(L);
          sol::function f = lua.load(bytes.as_string_view());
          f.call(w, singletonsView);
        }
        LuaMngr::Instance().Recycle(L);
      },
      std::move(name), std::move(singletonLocator));
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
