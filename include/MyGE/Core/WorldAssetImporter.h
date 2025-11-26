#pragma once

#include "../Core/AssetImporter.h"

namespace My::MyGE {
class WorldAsset {
 public:
  // use Serializer
  WorldAsset(const MyECS::World* world);
  WorldAsset(const MyECS::World* world, std::span<MyECS::Entity> entities);

  WorldAsset(std::string data) noexcept : data{std::move(data)} {}

  const std::string& GetData() const noexcept { return data; }

  // use Serializer
  bool ToWorld(MyECS::World* world);

 private:
  std::string data;
};

class WorldAssetImporter final : public TAssetImporter<WorldAssetImporter> {
 public:
  using TAssetImporter<WorldAssetImporter>::TAssetImporter;

  virtual std::string ReserializeAsset() const;
  virtual AssetImportContext ImportAsset() const override;

 private:
  friend class TAssetImporterCreator<WorldAssetImporter>;
  static void RegisterToMyDRefl();
};

class WorldAssetImporterCreator final
    : public TAssetImporterCreator<WorldAssetImporter> {
 public:
  virtual std::vector<std::string> SupportedExtentions() const override;
};
}  // namespace My::MyGE
