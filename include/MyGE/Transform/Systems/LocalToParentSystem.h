//
// Created by Admin on 13/03/2025.
//

#pragma once

#include <MyECS/World.h>
#include <MyGM/transform.h>

namespace My::MyGE {
class LocalToParentSystem : public MyECS::System {
 public:
  using System::System;

  static constexpr char SystemFuncName[] = "LocalToParentSystem";

  void ChildLocalToWorld(const transformf& parent_l2w, MyECS::Entity e);

  virtual void OnUpdate(MyECS::Schedule& schedule) override;
};
}  // namespace My::MyGE
