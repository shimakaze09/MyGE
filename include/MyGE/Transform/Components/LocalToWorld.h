//
// Created by Admin on 13/03/2025.
//

#pragma once

#include <MyGM/transform.h>

namespace My::MyGE {
struct LocalToWorld {
  transformf value{transformf::eye()};
};
}  // namespace My::MyGE

#include "details/LocalToWorld_AutoRefl.inl"
