//
// Created by Admin on 14/03/2025.
//

#include <MyECS/World.h>
#include <MyGE/ScriptSystem/LuaMngr.h>

#include <iostream>
using namespace std;

int main() {
  char buff[256];
  int error;
  My::MyGE::LuaMngr::Instance().Init();
  auto L = My::MyGE::LuaMngr::Instance().Request();
  {
    sol::state_view lua(L);
    lua.script_file("../assets/scripts/test_00.lua");
  }

  while (fgets(buff, sizeof(buff), stdin) != NULL) {
    error = luaL_loadstring(L, buff) || lua_pcall(L, 0, 0, 0);
    if (error) {
      fprintf(stderr, "%s\n", lua_tostring(L, -1));
      lua_pop(L, 1); /* pop error message from the stack */
    }
  }

  My::MyGE::LuaMngr::Instance().Recycle(L);

  My::MyECS::RTDCmptTraits::Instance().Clear();
  My::MyGE::LuaMngr::Instance().Clear();

  return 0;
}
