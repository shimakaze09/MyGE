#include <MyGE/Core/AssetImporter.h>
#include <MyGE/Core/AssetMngr.h>

using namespace Smkz::MyGE;

std::filesystem::path AssetImporter::GetFullPath() const {
  return AssetMngr::Instance().GetFullPath(
      AssetMngr::Instance().GUIDToAssetPath(GetGuid()));
}

std::string AssetImporter::ReserializeAsset() const {
  auto asset = AssetMngr::Instance().GUIDToAsset(guid);
  if (!asset.GetType() || !asset.GetPtr()) return {};
  return Serializer::Instance().Serialize(asset);
}

void AssetImporter::RegisterToMyDRefl() {
  if (MyDRefl::Mngr.typeinfos.contains(Type_of<AssetImporter>)) return;

  MyDRefl::Mngr.RegisterType<AssetImporter>();
  MyDRefl::Mngr.AddField<&AssetImporter::guid>(Serializer::Key::Guid);
}

AssetImportContext DefaultAssetImporter::ImportAsset() const {
  AssetImportContext ctx;

  auto path = GetFullPath();
  if (path.empty()) return {};
  std::string name = path.stem().string();
  ctx.AddObject(name, MyDRefl::SharedObject{Type_of<DefaultAsset>,
                                            std::make_shared<DefaultAsset>()});
  ctx.SetMainObjectID(name);
  return ctx;
}
