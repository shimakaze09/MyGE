#pragma once

#include "../../Core/SharedVar.h"
#include "../Material.h"

namespace My::MyGE {
// singleton
struct Skybox {
  SharedVar<Material> material;
};
}  // namespace My::MyGE

