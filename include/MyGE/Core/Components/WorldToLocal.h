#pragma once

#include <MyGM/transform.h>

namespace My::MyGE {
struct WorldToLocal {
  transformf value{transformf::eye()};
};
}  // namespace My::MyGE

#include "details/WorldToLocal_AutoRefl.inl"
