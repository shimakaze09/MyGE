#include <MyGE/Render/Texture2D.h>
#include <MyGE/Render/TextureCube.h>
#include <MyGE/Render/TextureImporter.h>

#include <filesystem>

using namespace Smkz::MyGE;

void TextureImporter::RegisterToMyDRefl() {
  RegisterToMyDReflHelper();

  MyDRefl::Mngr.RegisterType<TextureImporter::Mode>();
  MyDRefl::Mngr.SimpleAddField<TextureImporter::Mode::Texture2D>("Texture2D");
  MyDRefl::Mngr.SimpleAddField<TextureImporter::Mode::TextureCube>(
      "TextureCube");

  MyDRefl::Mngr.SimpleAddField<&TextureImporter::mode>("mode");
}

AssetImportContext TextureImporter::ImportAsset() const {
  AssetImportContext ctx;
  auto path = GetFullPath();
  if (path.empty()) return {};

  std::string name = path.stem().string();

  Image img(path.string());

  switch (mode) {
    case Smkz::MyGE::TextureImporter::Mode::Texture2D: {
      Texture2D t;
      t.image = std::move(img);
      auto tex = std::make_shared<Texture2D>(std::move(t));
      ctx.AddObject(name, MyDRefl::SharedObject{Type_of<Texture2D>, tex});
    } break;
    case Smkz::MyGE::TextureImporter::Mode::TextureCube: {
      auto tex = std::make_shared<TextureCube>(std::move(img));
      ctx.AddObject(name, MyDRefl::SharedObject{Type_of<TextureCube>, tex});
    } break;
    default:
      assert(false);
      return {};
  }

  ctx.SetMainObjectID(name);

  return ctx;
}

std::vector<std::string> TextureImporterCreator::SupportedExtentions() const {
  return {".png", ".bmp", ".tga", ".jpg", ".hdr"};
}
