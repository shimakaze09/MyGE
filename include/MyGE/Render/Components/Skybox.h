#pragma once

#include <MyGE/Core/Asset.h>

#include "../Material.h"

namespace Smkz::MyGE {
// singleton
struct Skybox {
  TAsset<Material> material;
};
}  // namespace Smkz::MyGE
