

#include "Texture2D.h"

#include "../Core/AssetImporter.h"
#include "../Core/SharedVar.h"

namespace My::MyGE {
struct TextureCubeData {
  SharedVar<Texture2D> pos_x;
  SharedVar<Texture2D> neg_x;
  SharedVar<Texture2D> pos_y;
  SharedVar<Texture2D> neg_y;
  SharedVar<Texture2D> pos_z;
  SharedVar<Texture2D> neg_z;
};

class TextureCubeImporter final : public TAssetImporter<TextureCubeImporter> {
 public:
  using TAssetImporter<TextureCubeImporter>::TAssetImporter;

  virtual AssetImportContext ImportAsset() const override;
  virtual std::string ReserializeAsset() const override;
  static void RegisterToMyDRefl();
};

class TextureCubeImporterCreator final
    : public TAssetImporterCreator<TextureCubeImporter> {
 public:
  virtual std::vector<std::string> SupportedExtentions() const override;
};
}  // namespace My::MyGE