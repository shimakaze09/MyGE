#pragma once

#include "../Core/AssetImporter.h"

namespace My::MyGE {
class ShaderImporter final : public TAssetImporter<ShaderImporter> {
 public:
  using TAssetImporter<ShaderImporter>::TAssetImporter;

  virtual AssetImportContext ImportAsset() const override;
  static void RegisterToMyDRefl();
};

class ShaderImporterCreator final
    : public TAssetImporterCreator<ShaderImporter> {
 public:
  virtual std::vector<std::string> SupportedExtentions() const override;
};
}  // namespace My::MyGE

