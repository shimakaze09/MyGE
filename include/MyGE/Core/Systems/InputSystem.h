//
// Created by Admin on 20/03/2025.
//

#pragma once

#include <MyECS/World.h>
#include <MyGM/transform.h>

namespace My::MyGE {
class InputSystem : public MyECS::System {
 public:
  using System::System;

  static constexpr char SystemFuncName[] = "InputSystem";

  virtual void OnUpdate(MyECS::Schedule& schedule) override;
};
}  // namespace My::MyGE
