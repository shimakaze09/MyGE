#pragma once

#include <MyGM/transform.hpp>

namespace My::MyGE {
struct Camera {
  float aspect{16.f / 9.f};
  [[interval(std::pair{1.f, 179.f})]]
  float fov{60.f};
  [[min(0.1f)]]
  float clippingPlaneMin{0.3f};
  [[min(0.1f)]]
  float clippingPlaneMax{1000.f};

  transformf prjectionMatrix;
};
}  // namespace My::MyGE

