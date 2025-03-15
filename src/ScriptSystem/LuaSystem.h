#pragma once

#include <MyECS/World.h>
#include <MyLuaPP/MyLuaPP.h>

namespace My::MyGE {
class LuaContext;

class LuaSystem : public MyECS::System {
 public:
  using System::System;

  static void RegisterSystem(MyECS::World* world, std::string name,
                             sol::function onUpdate);

  // World, SingletonsView, Entity, size_t index, CmptsView
  static const MyECS::SystemFunc* RegisterEntityJob(
      MyECS::Schedule*, sol::function systemFunc, std::string name,
      MyECS::ArchetypeFilter, MyECS::CmptLocator, MyECS::SingletonLocator);

  // World, SingletonsView, ChunkView
  static const MyECS::SystemFunc* RegisterChunkJob(MyECS::Schedule*,
                                                   sol::function systemFunc,
                                                   std::string name,
                                                   MyECS::ArchetypeFilter,
                                                   MyECS::SingletonLocator);

  // World, SingletonsView
  static const MyECS::SystemFunc* RegisterJob(MyECS::Schedule*,
                                              sol::function systemFunc,
                                              std::string name,
                                              MyECS::SingletonLocator);

 private:
  LuaSystem(MyECS::World* world, std::string name, sol::function onUpdate);

  virtual void OnUpdate(MyECS::Schedule& schedule) override;

  LuaContext* luaCtx;
  sol::function mainOnUpdate;
};
}  // namespace My::MyGE

#include "detail/LuaSystem_AutoRefl.inl"
#include "detail/MyECS_AutoRefl/System_AutoRefl.inl"
