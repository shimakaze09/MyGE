#pragma once

#include "../Core/Image.h"
#include "Texture.h"

namespace My::MyGE {
struct Texture2D : Texture {
  Texture2D() = default;

  Texture2D(Image image) : image{std::move(image)} {}

  Image image;
};
}  // namespace My::MyGE
