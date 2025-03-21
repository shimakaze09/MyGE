#pragma once

#include "../Material.h"

namespace My::MyGE {
// singleton
struct Skybox {
  const Material* material;
};
}  // namespace My::MyGE

#include "details/Skybox_AutoRefl.inl"
