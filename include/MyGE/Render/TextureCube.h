#pragma once

#include <MyDP/Basic/Read.h>

#include <array>

#include "Texture.h"

namespace My::MyGE {
class Image;

class TextureCube : public Texture {
 public:
  enum class SourceMode { SixSidedImages, EquirectangularMap };
  Read<TextureCube, SourceMode> mode;
  Read<TextureCube, std::array<std::shared_ptr<const Image>, 6>> images;
  Read<TextureCube, std::shared_ptr<const Image>> equirectangularMap;

  TextureCube(std::array<std::shared_ptr<const Image>, 6> images);
  TextureCube(std::shared_ptr<const Image> equirectangularMap);

  void Init(std::array<std::shared_ptr<const Image>, 6> images);
  void Init(std::shared_ptr<const Image> equirectangularMap);
  void Clear();
};
}  // namespace My::MyGE
