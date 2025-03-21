#include "InitMyECS.h"
#include "details/MyECS_AutoRefl/MyECS_AutoRefl.h"

void My::MyGE::detail::InitMyECS(lua_State* L) {
  MyLuaPP::Register<MyECS::AccessMode>(L);
  MyLuaPP::Register<MyECS::ArchetypeFilter>(L);
  MyLuaPP::Register<MyECS::ChunkView>(L);
  MyLuaPP::Register<MyECS::CmptLocator>(L);
  MyLuaPP::Register<MyECS::CmptPtr>(L);
  MyLuaPP::Register<MyECS::CmptAccessPtr>(L);
  MyLuaPP::Register<MyECS::CmptsView>(L);
  MyLuaPP::Register<MyECS::CmptType>(L);
  MyLuaPP::Register<MyECS::CmptAccessType>(L);
  MyLuaPP::Register<MyECS::Entity>(L);
  MyLuaPP::Register<MyECS::EntityMngr>(L);
  MyLuaPP::Register<MyECS::EntityQuery>(L);
  MyLuaPP::Register<MyECS::RTDCmptTraits>(L);
  MyLuaPP::Register<MyECS::Schedule>(L);
  MyLuaPP::Register<MyECS::SingletonLocator>(L);
  MyLuaPP::Register<MyECS::SingletonsView>(L);
  MyLuaPP::Register<MyECS::SystemMngr>(L);
  MyLuaPP::Register<MyECS::World>(L);
}
