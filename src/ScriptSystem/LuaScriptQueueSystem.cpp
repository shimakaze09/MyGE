#include <MyGE/ScriptSystem/LuaContext.h>
#include <MyGE/ScriptSystem/LuaCtxMngr.h>
#include <MyGE/ScriptSystem/LuaScript.h>
#include <MyGE/ScriptSystem/LuaScriptQueue.h>
#include <MyGE/ScriptSystem/LuaScriptQueueSystem.h>
#include <_deps/spdlog/spdlog.h>

#include <_deps/sol/sol.hpp>

using namespace My::MyGE;
using namespace My::MyECS;

void LuaScriptQueueSystem::OnUpdate(Schedule& schedule) {
  schedule.RegisterCommand([](World* w) {
    auto scripts = w->entityMngr.GetSingleton<LuaScriptQueue>();
    if (!scripts)
      return;

    sol::state_view lua{LuaCtxMngr::Instance().GetContext(w)->Main()};
    for (auto script : scripts->value) {
      if (!script)
        continue;
      auto rst = lua.safe_script(script->GetText());
      if (!rst.valid()) {
        sol::error err = rst;
        spdlog::error(err.what());
      }
    }
    scripts->value.clear();
  });
}
