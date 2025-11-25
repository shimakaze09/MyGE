#pragma once

#include "../Core/Image.h"
#include "Texture.h"

namespace My::MyGE {
struct Texture2D : Texture {
  Image image;
};
}  // namespace My::MyGE

