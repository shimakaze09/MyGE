//
// Created by Admin on 17/03/2025.
//

#include "InitCore.h"

#include <MyGE/Core/Components/Camera.h>
#include <MyGE/Core/Components/MeshFilter.h>
#include <MyGE/Core/Components/MeshRenderer.h>
#include <MyGE/Core/Components/WorldTime.h>

void My::MyGE::detail::InitCore(lua_State* L) {
  MyLuaPP::Register<Camera>(L);
  MyLuaPP::Register<MeshFilter>(L);
  MyLuaPP::Register<MeshRenderer>(L);
  MyLuaPP::Register<WorldTime>(L);
}
