#include <MyGE/Core/AssetImporter.h>
#include <MyGE/Core/AssetMngr.h>

using namespace Smkz::MyGE;

void AssetImporter::RegisterToMyDRefl() {
  if (MyDRefl::Mngr.typeinfos.contains(Type_of<AssetImporter>)) return;

  MyDRefl::Mngr.RegisterType<AssetImporter>();
  MyDRefl::Mngr.AddField<&AssetImporter::guid>(Serializer::Key::Guid);
}

AssetImportContext DefaultAssetImporter::ImportAsset() const {
  AssetImportContext ctx;
  const auto& path = AssetMngr::Instance().GUIDToAssetPath(GetGuid());
  if (path.empty()) return {};
  std::string name = path.stem().string();
  ctx.AddObject(name, MyDRefl::SharedObject{Type_of<DefaultAsset>,
                                            std::make_shared<DefaultAsset>()});
  ctx.SetMainObjectID(name);
  return ctx;
}
