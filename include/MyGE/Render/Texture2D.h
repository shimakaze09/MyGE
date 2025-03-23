#pragma once

#include "Texture.h"

#include <memory>

namespace My::MyGE {
class Image;

struct Texture2D : Texture {
  std::shared_ptr<const Image> image;
};
}  // namespace My::MyGE
