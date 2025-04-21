#pragma once

#include <vector>

#include "../../Core/SharedVar.h"
#include "../Material.h"

namespace Smkz::MyGE {
struct MeshRenderer {
  using allocator_type = std::pmr::vector<SharedVar<Material>>::allocator_type;
  MeshRenderer(const allocator_type& alloc) : materials(alloc) {}
  MeshRenderer(const MeshRenderer& other, const allocator_type& alloc)
      : materials(other.materials, alloc) {}
  MeshRenderer(MeshRenderer&& other, const allocator_type& alloc)
      : materials(std::move(other.materials), alloc) {}

  std::pmr::vector<SharedVar<Material>> materials;
};
}  // namespace Smkz::MyGE
