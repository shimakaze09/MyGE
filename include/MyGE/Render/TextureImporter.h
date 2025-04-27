#pragma once

#include "../Core/AssetImporter.h"

namespace Smkz::MyGE {
class TextureImporter final : public TAssetImporter<TextureImporter> {
 public:
  using TAssetImporter<TextureImporter>::TAssetImporter;

  virtual AssetImportContext ImportAsset() const override;
  static void RegisterToMyDRefl();

  enum class Mode { Texture2D, TextureCube };

  Mode mode{Mode::Texture2D};
};

class TextureImporterCreator final
    : public TAssetImporterCreator<TextureImporter> {
 public:
  virtual std::vector<std::string> SupportedExtentions() const override;
};
}  // namespace Smkz::MyGE
