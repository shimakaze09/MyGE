//
// Created by Admin on 16/03/2025.
//

#pragma once

#include <MyECS/World.h>
#include <MyGM/transform.h>

namespace My::MyGE {
class CameraSystem : public MyECS::System {
 public:
  using System::System;

  static constexpr char SystemFuncName[] = "CameraSystem";

  virtual void OnUpdate(MyECS::Schedule& schedule) override;
};
}  // namespace My::MyGE
