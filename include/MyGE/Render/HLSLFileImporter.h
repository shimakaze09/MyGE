#pragma once

#include "../Core/AssetImporter.h"

namespace My::MyGE {
class HLSLFileImporter final : public TAssetImporter<HLSLFileImporter> {
 public:
  using TAssetImporter<HLSLFileImporter>::TAssetImporter;

  virtual AssetImportContext ImportAsset() const override;
  static void RegisterToMyDRefl();
};

class HLSLFileImporterCreator final
    : public TAssetImporterCreator<HLSLFileImporter> {
 public:
  virtual std::vector<std::string> SupportedExtentions() const override;
};
}  // namespace My::MyGE

