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

  static void RegisterChunkFunc(MyECS::Schedule*, sol::function onUpdate,
                                std::string name, MyECS::EntityFilter);

  static std::function<void(MyECS::ChunkView)> WrapChunkFunc(sol::function);

 private:
  LuaSystem(MyECS::World* world, std::string name, sol::function onUpdate);

  virtual void OnUpdate(MyECS::Schedule& schedule) override;

  sol::bytecode onUpdate;
};
}  // namespace My::MyGE

#include "detail/LuaSystem_AutoRefl.inl"
#include "detail/MyECS_Refl/System_AutoRefl.inl"
