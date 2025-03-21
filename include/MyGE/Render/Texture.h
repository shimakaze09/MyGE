#pragma once

#include "RenderRsrcObject.h"

namespace My::MyGE {
class Texture : public RenderRsrcObject {
 public:
  virtual ~Texture() = default;

 protected:
  Texture() = default;
};
}  // namespace My::MyGE
