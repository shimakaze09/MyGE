#pragma once

#include "../Material.h"
#include <MySTL/memory.h>

namespace My::MyGE {
// singleton
struct Skybox {
  MySTL::shared_object<Material> material;
};
}  // namespace My::MyGE

#include "details/Skybox_AutoRefl.inl"
