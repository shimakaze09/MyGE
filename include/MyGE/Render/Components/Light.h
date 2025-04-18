#pragma once

#include <MyGM/rgb.h>

namespace My::MyGE {
struct Light {
  enum class Type { Directional, Point, Spot, Rect, Disk };

  Type type{Type::Directional};
  rgbf color{1.f};
  float intensity{1.f};
  float range{10.f};
  float width{1.f};
  float height{1.f};
  float innerSpotAngle{21.802f};
  float outerSpotAngle{30.f};
};
}  // namespace My::MyGE

#include "details/Light_AutoRefl.inl"
