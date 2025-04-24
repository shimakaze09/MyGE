#pragma once

#include "../../Core/SharedVar.h"
#include "../Material.h"

namespace Smkz::MyGE {
// singleton
struct Skybox {
  SharedVar<Material> material;
};
}  // namespace Smkz::MyGE
