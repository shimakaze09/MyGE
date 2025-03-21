#include "InitCore.h"

#include <MyGE/Core/Components/Components.h>

void My::MyGE::detail::InitCore(lua_State* L) {
  MyLuaPP::Register<Children>(L);
  MyLuaPP::Register<Input>(L);
  MyLuaPP::Register<LocalToParent>(L);
  MyLuaPP::Register<LocalToWorld>(L);
  MyLuaPP::Register<Name>(L);
  MyLuaPP::Register<Parent>(L);
  MyLuaPP::Register<Roamer>(L);
  MyLuaPP::Register<Rotation>(L);
  MyLuaPP::Register<RotationEuler>(L);
  MyLuaPP::Register<Scale>(L);
  MyLuaPP::Register<Translation>(L);
  MyLuaPP::Register<WorldTime>(L);
  MyLuaPP::Register<WorldToLocal>(L);
}
