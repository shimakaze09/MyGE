#pragma once

#include "../Mesh.h"
#include <MySTL/memory.h>

namespace My::MyGE {
struct MeshFilter {
  MySTL::shared_object<Mesh> mesh;
};
}  // namespace My::MyGE

#include "details/MeshFilter_AutoRefl.inl"
