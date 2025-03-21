#pragma once

#include <MyGM/vec.h>

namespace My::MyGE {
struct Translation {
  vecf3 value{0.f};
};
}  // namespace My::MyGE

#include "details/Translation_AutoRefl.inl"
