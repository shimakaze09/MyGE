#pragma once

namespace Smkz::MyGE {
struct Roamer {
  float moveSpeed{1.f};
  float rotateSpeed{1.f};
  bool reverseUpDown{false};
  bool reverseLeftRight{false};
  bool reverseFrontBack{false};
};
}  // namespace Smkz::MyGE
