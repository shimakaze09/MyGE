#pragma once

#include <vector>

#include "../Material.h"
#include <MySTL/memory.h>

namespace My::MyGE {
struct MeshRenderer {
  std::vector<MySTL::shared_object<Material>> materials;
};
}  // namespace My::MyGE

#include "details/MeshRenderer_AutoRefl.inl"
