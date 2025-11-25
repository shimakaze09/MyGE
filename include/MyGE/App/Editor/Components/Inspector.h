#pragma once

#include <MyECS/Entity.hpp>
#include <_deps/crossguid/guid.hpp>

namespace My::MyGE {
struct Inspector {
  enum class Mode { Entity, Asset };

  bool lock{false};
  Mode mode;
  MyECS::Entity entity{MyECS::Entity::Invalid()};
  xg::Guid asset;
};
}  // namespace My::MyGE

