#pragma once

#include <vector>

#include "../Material.h"

namespace My::MyGE {
struct MeshRenderer {
  std::vector<std::shared_ptr<Material>> materials;
};
}  // namespace My::MyGE

#include "details/MeshRenderer_AutoRefl.inl"
