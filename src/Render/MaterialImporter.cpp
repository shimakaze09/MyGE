#include <MyGE/Render/Material.h>
#include <MyGE/Render/MaterialImporter.h>
#include <MyGE/Render/Shader.h>

#include <filesystem>

#include "ShaderImporter/ShaderImporterRegisterImpl.h"

using namespace My::MyGE;

void MaterialImporter::RegisterToMyDRefl() {
  RegisterToMyDReflHelper();

  // Register ShaderProperty and map
  details::ShaderImporterRegister_Shader();

  MyDRefl::Mngr.RegisterType<Material>();
  MyDRefl::Mngr.AddField<&Material::shader>("shader");
  MyDRefl::Mngr.AddField<&Material::properties>("properties");
}

AssetImportContext MaterialImporter::ImportAsset() const {
  AssetImportContext ctx;
  auto path = GetFullPath();
  if (path.empty())
    return {};

  std::ifstream ifs(path);
  assert(ifs.is_open());
  std::string str;

  ifs.seekg(0, std::ios::end);
  str.reserve(ifs.tellg());
  ifs.seekg(0, std::ios::beg);

  str.assign(std::istreambuf_iterator<char>(ifs),
             std::istreambuf_iterator<char>());

  auto material = std::static_pointer_cast<Material>(
      Serializer::Instance().Deserialize(str).GetBuffer());

  ctx.AddObject("main", MyDRefl::SharedObject{Type_of<Material>, material});
  ctx.SetMainObjectID("main");

  return ctx;
}

std::vector<std::string> MaterialImporterCreator::SupportedExtentions() const {
  return {".mat"};
}
