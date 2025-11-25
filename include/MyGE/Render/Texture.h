#pragma once

#include "GPURsrc.h"

namespace My::MyGE {
class Texture : public GPURsrc {
 public:
  virtual ~Texture() = default;

 protected:
  Texture() = default;
};
}  // namespace My::MyGE

