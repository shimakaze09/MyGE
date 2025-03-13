//
// Created by Admin on 13/03/2025.
//

#pragma once

#include <MyGM/euler.h>

namespace My::MyGE {
struct RotationEuler {
  eulerf value{0.f};
};
}  // namespace My::MyGE

#include "details/RotationEuler_AutoRefl.inl"
