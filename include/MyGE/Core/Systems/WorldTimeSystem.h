//
// Created by Admin on 16/03/2025.
//

#pragma once

#include <MyECS/World.h>

namespace My::MyGE {
class WorldTimeSystem : public MyECS::System {
 public:
  using MyECS::System::System;

  static constexpr char SystemFuncName[] = "WorldTimeSystem";

  virtual void OnUpdate(MyECS::Schedule& schedule) override;
};
}  // namespace My::MyGE
