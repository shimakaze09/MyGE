//
// Created by Admin on 19/03/2025.
//

#pragma once

#include "../Material.h"

namespace My::MyGE {
// singleton
struct Skybox {
  const Material* material;
};
}  // namespace My::MyGE

#include "details/Skybox_AutoRefl.inl"
