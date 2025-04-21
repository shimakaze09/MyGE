#pragma once

#include "../../Core/SharedVar.h"
#include "../Mesh.h"

namespace Smkz::MyGE {
struct MeshFilter {
  SharedVar<Mesh> mesh;
};
}  // namespace Smkz::MyGE
