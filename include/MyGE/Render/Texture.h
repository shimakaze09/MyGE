#pragma once

#include "../Core/Object.h"

namespace My::MyGE {
class Texture : public Object {
 public:
  virtual ~Texture() = default;

 protected:
  Texture() = default;
};
}  // namespace My::MyGE
