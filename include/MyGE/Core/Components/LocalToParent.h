#pragma once

#include <MyGM/transform.h>

namespace My::MyGE {
struct LocalToParent {
  transformf value{transformf::eye()};
};
}  // namespace My::MyGE

#include "details/LocalToParent_AutoRefl.inl"
