#pragma once

#include "Texture.h"

namespace My::MyGE {
class Image;

struct Texture2D : Texture {
  enum class WrapMode { Clamp, Repeat, Mirror };
  enum class FilterMode { Point, Bilinear };

  const Image* image;
  WrapMode wrapMode{WrapMode::Repeat};
  FilterMode filterMode{FilterMode::Bilinear};
};
}  // namespace My::MyGE
