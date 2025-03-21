//
// Created by Admin on 13/03/2025.
//

#pragma once

#include <MyECS/World.h>
#include <MyGM/transform.h>

namespace My::MyGE {
struct LocalToParentSystem {
  static constexpr char SystemFuncName[] = "LocalToParentSystem";

  static void ChildLocalToWorld(MyECS::World* w, const transformf& parent_l2w,
                                MyECS::Entity e);

  static void OnUpdate(MyECS::Schedule& schedule);
};
}  // namespace My::MyGE
