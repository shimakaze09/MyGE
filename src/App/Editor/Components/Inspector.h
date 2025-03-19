//
// Created by Admin on 19/03/2025.
//

#pragma once

#include <MyECS/Entity.h>
#include <_deps/crossguid/guid.hpp>

namespace My::MyGE {
struct Inspector {
  enum class Mode { Entity, Asset };

  bool lock{false};
  Mode mode;
  MyECS::Entity entity;
  xg::Guid asset;
};
}  // namespace My::MyGE
