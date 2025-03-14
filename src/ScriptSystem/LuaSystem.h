//
// Created by Admin on 14/03/2025.
//

#pragma once

#include <MyECS/World.h>
#include <MyLuaPP/MyLuaPP.h>

namespace My::MyGE {
class LuaSystem : public MyECS::System {
 public:
  using System::System;

  static void Register(MyECS::World* world, std::string name,
                       sol::function onUpdate);

  // World, Entity, size_t index, CmptsView
  static const MyECS::SystemFunc* RegisterSystemFunc_Entity(
      MyECS::Schedule*, sol::function systemFunc, std::string name,
      MyECS::EntityLocator, MyECS::EntityFilter);

  // World, ChunkView
  static const MyECS::SystemFunc* RegisterSystemFunc_Chunk(
      MyECS::Schedule*, sol::function systemFunc, std::string name,
      MyECS::EntityFilter);

  // World
  static const MyECS::SystemFunc* RegisterSystemFunc_Job(
      MyECS::Schedule*, sol::function systemFunc, std::string name);

 private:
  LuaSystem(MyECS::World* world, std::string name, sol::function onUpdate);

  virtual void OnUpdate(MyECS::Schedule& schedule) override;

  sol::bytecode onUpdate;
};
}  // namespace My::MyGE

#include "detail/LuaSystem_AutoRefl.inl"
#include "detail/MyECS_AutoRefl/System_AutoRefl.inl"
