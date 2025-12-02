#pragma once

#include "../Core/AssetImporter.h"

namespace My::MyGE {
class TextureImporter final : public TAssetImporter<TextureImporter> {
 public:
  using TAssetImporter<TextureImporter>::TAssetImporter;

  virtual AssetImportContext ImportAsset() const override;
  virtual std::string ReserializeAsset() const;

  enum class Mode { Texture2D, TextureCube };

  Mode mode{Mode::Texture2D};

 private:
  friend class TAssetImporterCreator<TextureImporter>;
  static void RegisterToMyDRefl();
};

class TextureImporterCreator final
    : public TAssetImporterCreator<TextureImporter> {
 public:
  virtual std::vector<std::string> SupportedExtentions() const override;
};
}  // namespace My::MyGE
