#pragma once

#include "../../Core/SharedVar.h"
#include "../Mesh.h"

namespace My::MyGE {
struct MeshFilter {
  SharedVar<Mesh> mesh;
};
}  // namespace My::MyGE

