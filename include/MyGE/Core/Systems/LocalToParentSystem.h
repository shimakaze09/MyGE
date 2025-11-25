#pragma once

#include <MyECS/MyECS.hpp>
#include <MyGM/transform.hpp>

namespace My::MyGE {
struct LocalToParentSystem {
  static constexpr char SystemFuncName[] = "LocalToParentSystem";

  static void ChildLocalDeserializeToWorld(MyECS::World* w,
                                           const transformf& parent_l2w,
                                           MyECS::Entity e);

  static void OnUpdate(MyECS::Schedule& schedule);
};
}  // namespace My::MyGE

