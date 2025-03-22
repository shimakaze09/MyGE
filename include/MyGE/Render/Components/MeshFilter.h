#pragma once

#include "../Mesh.h"

namespace My::MyGE {
struct MeshFilter {
  std::shared_ptr<Mesh> mesh;
};
}  // namespace My::MyGE

#include "details/MeshFilter_AutoRefl.inl"
