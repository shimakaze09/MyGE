//
// Created by Admin on 14/03/2025.
//

#include "InitMyECS.h"

#include "detail/MyECS_Refl/MyECS_Refl.h"

void My::MyGE::detail::InitMyECS(lua_State* L) {
  MyLuaPP::Register<MyECS::AccessMode>(L);
  MyLuaPP::Register<MyECS::ChunkView>(L);
  MyLuaPP::Register<MyECS::CmptPtr>(L);
  MyLuaPP::Register<MyECS::CmptsView>(L);
  MyLuaPP::Register<MyECS::CmptType>(L);
  MyLuaPP::Register<MyECS::EntityFilter>(L);
  MyLuaPP::Register<MyECS::EntityLocator>(L);
  MyLuaPP::Register<MyECS::EntityMngr>(L);
  MyLuaPP::Register<MyECS::EntityQuery>(L);
  MyLuaPP::Register<MyECS::Entity>(L);
  MyLuaPP::Register<MyECS::RTDCmptTraits>(L);
  MyLuaPP::Register<MyECS::SystemMngr>(L);
  MyLuaPP::Register<MyECS::World>(L);
}
