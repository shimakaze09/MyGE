#pragma once

#include <MyECS/World.h>
#include <MyLuaPP/MyLuaPP.h>

namespace My::MyGE {
struct LuaECSAgency {
  // World, SingletonsView
  static std::function<void(MyECS::Schedule&)> SafeOnUpdate(
      sol::function onUpdate);

  // World, SingletonsView, Entity, size_t index, CmptsView
  static const MyECS::SystemFunc* RegisterEntityJob(
      MyECS::Schedule*, sol::function systemFunc, std::string name,
      bool isParallel = true, MyECS::ArchetypeFilter = {},
      MyECS::CmptLocator = {}, MyECS::SingletonLocator = {},
      MyECS::RandomAccessor = {});

  // World, SingletonsView, entityBeginIndexInQuery, ChunkView
  static const MyECS::SystemFunc* RegisterChunkJob(
      MyECS::Schedule*, sol::function systemFunc, std::string name,
      MyECS::ArchetypeFilter = {}, bool isParallel = true,
      MyECS::SingletonLocator = {}, MyECS::RandomAccessor = {});

  // World, SingletonsView
  static const MyECS::SystemFunc* RegisterJob(MyECS::Schedule*,
                                              sol::function systemFunc,
                                              std::string name,
                                              MyECS::SingletonLocator = {},
                                              MyECS::RandomAccessor = {});
};
}  // namespace My::MyGE

#include "detail/LuaECSAgency_AutoRefl.inl"
