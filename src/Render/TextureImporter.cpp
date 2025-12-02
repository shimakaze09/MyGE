#include <MyGE/Render/Texture2D.h>
#include <MyGE/Render/TextureCube.h>
#include <MyGE/Render/TextureImporter.h>

#include <MyGE/Core/AssetMngr.h>

#include <filesystem>

using namespace My::MyGE;

void TextureImporter::RegisterToMyDRefl() {
  RegisterToMyDReflHelper();

  MyDRefl::Mngr.RegisterType<TextureImporter::Mode>();
  MyDRefl::Mngr.AddField<TextureImporter::Mode::Texture2D>("Texture2D");
  MyDRefl::Mngr.AddField<TextureImporter::Mode::TextureCube>("TextureCube");

  MyDRefl::Mngr.RegisterType<Texture2D>();
  MyDRefl::Mngr.RegisterType<TextureCube>();

  MyDRefl::Mngr.AddField<&TextureImporter::mode>("mode");
}

AssetImportContext TextureImporter::ImportAsset() const {
  AssetImportContext ctx;
  auto path = GetFullPath();
  if (path.empty())
    return {};

  Image img(path.string());

  switch (mode) {
    case My::MyGE::TextureImporter::Mode::Texture2D: {
      Texture2D t;
      t.image = std::move(img);
      auto tex = std::make_shared<Texture2D>(std::move(t));
      ctx.AddObject("main", MyDRefl::SharedObject{Type_of<Texture2D>, tex});
    } break;
    case My::MyGE::TextureImporter::Mode::TextureCube: {
      auto tex = std::make_shared<TextureCube>(std::move(img));
      ctx.AddObject("main", MyDRefl::SharedObject{Type_of<TextureCube>, tex});
    } break;
    default:
      assert(false);
      return {};
  }

  ctx.SetMainObjectID("main");

  return ctx;
}

std::string TextureImporter::ReserializeAsset() const {
  auto asset = AssetMngr::Instance().GUIDToMainAsset(GetGuid());
  if (!asset.GetPtr())
    return {};

  const auto path = GetFullPath();
  if (mode == Mode::Texture2D) {
    if (!asset.GetType().Is<Texture2D>())
      return {};

    asset.As<Texture2D>().image.Save(path.string());
  } else if (mode == Mode::TextureCube) {
    if (!asset.GetType().Is<TextureCube>())
      return {};
    if (asset.As<TextureCube>().GetSourceMode() !=
        TextureCube::SourceMode::EquirectangularMap)
      return {};
    asset.As<TextureCube>().GetEquiRectangularMap().Save(path.string());
  } else
    assert(false);

  return {};
}

std::vector<std::string> TextureImporterCreator::SupportedExtentions() const {
  return {".png", ".bmp", ".tga", ".jpg", ".hdr"};
}
