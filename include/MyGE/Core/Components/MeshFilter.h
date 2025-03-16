//
// Created by Admin on 16/03/2025.
//

#pragma once

#include <vector>

#include "../Mesh.h"

namespace My::MyGE {
struct MeshFilter {
  Mesh* mesh{nullptr};
};
}  // namespace My::MyGE

#include "details/MeshFilter_AutoRefl.inl"
