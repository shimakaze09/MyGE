#pragma once

#include "../Core/AssetImporter.h"

namespace My::MyGE {
class MaterialImporter final : public TAssetImporter<MaterialImporter> {
 public:
  using TAssetImporter<MaterialImporter>::TAssetImporter;

  virtual AssetImportContext ImportAsset() const override;
  static void RegisterToMyDRefl();
};

class MaterialImporterCreator final
    : public TAssetImporterCreator<MaterialImporter> {
 public:
  virtual std::vector<std::string> SupportedExtentions() const override;
};
}  // namespace My::MyGE

