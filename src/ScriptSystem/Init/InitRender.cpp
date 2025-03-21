#include "InitRender.h"

#include <MyGE/Render/Components/Components.h>

void My::MyGE::detail::InitRender(lua_State* L) {
  MyLuaPP::Register<Camera>(L);
  MyLuaPP::Register<Light>(L);
  MyLuaPP::Register<MeshFilter>(L);
  MyLuaPP::Register<MeshRenderer>(L);
  MyLuaPP::Register<Skybox>(L);
}
