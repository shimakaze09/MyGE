#pragma once

#include "../Material.h"

namespace My::MyGE {
// singleton
struct Skybox {
  std::shared_ptr<Material> material;
};
}  // namespace My::MyGE

#include "details/Skybox_AutoRefl.inl"
