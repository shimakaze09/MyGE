#include <MyECS/World.h>
#include <MyGE/ScriptSystem/LuaContext.h>
#include <MyGE/ScriptSystem/LuaCtxMngr.h>
#include <MyLuaPP/MyLuaPP.h>

#include <iostream>
using namespace std;

int main() {
  char buff[256];
  int error;
  {
    My::MyECS::World world;
    auto luaCtx = My::MyGE::LuaCtxMngr::Instance().Register(&world);
    auto L = luaCtx->Main();
    {
      sol::state_view lua(L);
      lua["world"] = &world;
      auto rst = lua.script_file("../assets/scripts/test_00.lua");
      if (!rst.valid()) {
        sol::error err = rst;
        std::cerr << err.what() << std::endl;
      }
    }

    auto cmptType0 = My::MyECS::CmptType{"Cmpt0"};
    auto cmptType1 = My::MyECS::CmptType{"Cmpt1"};
    std::array cmptTypes = {cmptType0, cmptType1};
    auto e0 = world.entityMngr.Create(cmptTypes);
    auto e1 = world.entityMngr.Create(cmptTypes);
    auto e2 = world.entityMngr.Create(cmptType0);
    world.entityMngr.Detach(e0, cmptType0);

    world.Update();
    std::cout << world.DumpUpdateJobGraph() << std::endl;
    std::cout << world.GenUpdateFrameGraph().Dump() << std::endl;

    while (fgets(buff, sizeof(buff), stdin) != NULL) {
      error = luaL_loadstring(L, buff) || lua_pcall(L, 0, 0, 0);
      if (error) {
        fprintf(stderr, "%s\n", lua_tostring(L, -1));
        lua_pop(L, 1); /* pop error message from the stack */
      }
    }
  }

  My::MyGE::LuaCtxMngr::Instance().Clear();

  return 0;
}
