#pragma once

#include "GPURsrc.h"

namespace Smkz::MyGE {
class Texture : public GPURsrc {
 public:
  virtual ~Texture() = default;

 protected:
  Texture() = default;
};
}  // namespace Smkz::MyGE
