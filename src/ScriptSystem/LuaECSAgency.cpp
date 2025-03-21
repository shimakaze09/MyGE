//
// Created by Admin on 20/03/2025.
//

#include "LuaECSAgency.h"

#include <MyGE/ScriptSystem/LuaContext.h>
#include <MyGE/ScriptSystem/LuaCtxMngr.h>

using namespace My::MyGE;

const My::MyECS::SystemFunc* LuaECSAgency::RegisterEntityJob(
    MyECS::Schedule* s, sol::function systemFunc, std::string name,
    MyECS::ArchetypeFilter filter, MyECS::CmptLocator cmptLocator,
    MyECS::SingletonLocator singletonLocator, bool isParallel) {
  assert(!cmptLocator.CmptAccessTypes().empty());
  auto bytes = systemFunc.dump();
  auto sysfunc = s->RegisterChunkJob(
      [bytes = std::move(bytes), cmptLocator = std::move(cmptLocator)](
          MyECS::World* w, MyECS::SingletonsView singletonsView,
          MyECS::ChunkView chunk) {
        if (chunk.EntityNum() == 0)
          return;

        auto luaCtx = LuaCtxMngr::Instance().GetContext(w);
        auto L = luaCtx->Request();
        {
          sol::state_view lua(L);
          sol::function f = lua.load(bytes.as_string_view());

          auto arrayEntity = chunk.GetCmptArray<MyECS::Entity>();
          std::vector<void*> cmpts;
          std::vector<MyECS::CmptAccessType> types;
          std::vector<MyECS::CmptAccessPtr> cmptPtrs;
          std::vector<size_t> sizes;
          cmpts.reserve(cmptLocator.CmptAccessTypes().size());
          types.reserve(cmptLocator.CmptAccessTypes().size());
          cmptPtrs.reserve(cmptLocator.CmptAccessTypes().size());
          sizes.reserve(cmptLocator.CmptAccessTypes().size());
          for (const auto& t : cmptLocator.CmptAccessTypes()) {
            cmpts.push_back(chunk.GetCmptArray(t));
            types.push_back(t);
            cmptPtrs.emplace_back(t, cmpts.back());
            sizes.push_back(w->entityMngr.cmptTraits.Sizeof(t));
          }

          size_t i = 0;
          do {
            MyECS::CmptsView view{cmptPtrs.data(), cmptPtrs.size()};
            f.call(w, singletonsView, arrayEntity[i], i, view);
            for (size_t j = 0; j < cmpts.size(); j++) {
              cmpts[j] = (reinterpret_cast<uint8_t*>(cmpts[j]) + sizes[j]);
              cmptPtrs[j] = {types[j], cmpts[j]};
            }
          } while (++i < chunk.EntityNum());
        }
        luaCtx->Recycle(L);
      },
      std::move(name), std::move(filter), isParallel,
      std::move(singletonLocator));
  return sysfunc;
}

const My::MyECS::SystemFunc* LuaECSAgency::RegisterChunkJob(
    MyECS::Schedule* s, sol::function systemFunc, std::string name,
    MyECS::ArchetypeFilter filter, MyECS::SingletonLocator singletonLocator,
    bool isParallel) {
  auto bytes = systemFunc.dump();
  auto sysfunc = s->RegisterChunkJob(
      [bytes](MyECS::World* w, MyECS::SingletonsView singletonsView,
              MyECS::ChunkView chunk) {
        auto luaCtx = LuaCtxMngr::Instance().GetContext(w);
        auto L = luaCtx->Request();
        {
          sol::state_view lua(L);
          sol::function f = lua.load(bytes.as_string_view());
          f.call(w, singletonsView, chunk);
        }
        luaCtx->Recycle(L);
      },
      std::move(name), std::move(filter), isParallel,
      std::move(singletonLocator));
  return sysfunc;
}

const My::MyECS::SystemFunc* LuaECSAgency::RegisterJob(
    MyECS::Schedule* s, sol::function systemFunc, std::string name,
    MyECS::SingletonLocator singletonLocator) {
  auto bytes = systemFunc.dump();
  auto sysfunc = s->RegisterJob(
      [bytes](MyECS::World* w, MyECS::SingletonsView singletonsView) {
        auto luaCtx = LuaCtxMngr::Instance().GetContext(w);
        auto L = luaCtx->Request();
        {
          sol::state_view lua(L);
          sol::function f = lua.load(bytes.as_string_view());
          f.call(w, singletonsView);
        }
        luaCtx->Recycle(L);
      },
      std::move(name), std::move(singletonLocator));
  return sysfunc;
}
