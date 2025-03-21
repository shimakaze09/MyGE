#pragma once

#include <vector>

#include "../Material.h"

namespace My::MyGE {
struct MeshRenderer {
  std::vector<const Material*> materials;
};
}  // namespace My::MyGE

#include "details/MeshRenderer_AutoRefl.inl"
