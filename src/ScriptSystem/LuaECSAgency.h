#pragma once

#include <MyECS/World.h>
#include <MyLuaPP/MyLuaPP.h>

namespace My::MyGE {
struct LuaECSAgency {
  // World, SingletonsView, Entity, size_t index, CmptsView
  static const MyECS::SystemFunc* RegisterEntityJob(
      MyECS::Schedule*, sol::function systemFunc, std::string name,
      MyECS::ArchetypeFilter, MyECS::CmptLocator, MyECS::SingletonLocator,
      bool isParallel);

  // World, SingletonsView, ChunkView
  static const MyECS::SystemFunc* RegisterChunkJob(
      MyECS::Schedule*, sol::function systemFunc, std::string name,
      MyECS::ArchetypeFilter, MyECS::SingletonLocator, bool isParallel);

  // World, SingletonsView
  static const MyECS::SystemFunc* RegisterJob(MyECS::Schedule*,
                                              sol::function systemFunc,
                                              std::string name,
                                              MyECS::SingletonLocator);
};
}  // namespace My::MyGE

#include "detail/LuaECSAgency_AutoRefl.inl"
