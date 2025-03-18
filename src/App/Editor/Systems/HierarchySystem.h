//
// Created by Admin on 19/03/2025.
//

#pragma once

#include <MyECS/World.h>

namespace My::MyGE {
class HierarchySystem : public MyECS::System {
 public:
  using MyECS::System::System;

 private:
  virtual void OnUpdate(MyECS::Schedule&) override;
};
}  // namespace My::MyGE
