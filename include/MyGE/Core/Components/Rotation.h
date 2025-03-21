#pragma once

#include <MyGM/quat.h>

namespace My::MyGE {
struct Rotation {
  quatf value{quatf::identity()};
};
}  // namespace My::MyGE

#include "details/Rotation_AutoRefl.inl"
