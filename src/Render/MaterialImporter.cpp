#include <MyGE/Render/Material.h>
#include <MyGE/Render/MaterialImporter.h>

#include <filesystem>

#include "ShaderImporter/ShaderImporterRegisterImpl.h"

using namespace Smkz::MyGE;

void MaterialImporter::RegisterToMyDRefl() {
  RegisterToMyDReflHelper();

  // Register ShaderProperty and map
  details::ShaderImporterRegister_Shader();

  MyDRefl::Mngr.RegisterType<Material>();
  MyDRefl::Mngr.AddField<&Material::shader>("shader");
  MyDRefl::Mngr.SimpleAddField<&Material::properties>("properties");
}

AssetImportContext MaterialImporter::ImportAsset() const {
  AssetImportContext ctx;
  auto path = GetFullPath();
  if (path.empty()) return {};

  std::string name = path.stem().string();

  std::ifstream ifs(path);
  assert(ifs.is_open());
  std::string str;

  ifs.seekg(0, std::ios::end);
  str.reserve(ifs.tellg());
  ifs.seekg(0, std::ios::beg);

  str.assign(std::istreambuf_iterator<char>(ifs),
             std::istreambuf_iterator<char>());

  auto materail = Serializer::Instance().Deserialize(str).AsShared<Material>();

  ctx.AddObject(name, MyDRefl::SharedObject{Type_of<Material>, materail});
  ctx.SetMainObjectID(name);

  return ctx;
}

std::vector<std::string> MaterialImporterCreator::SupportedExtentions() const {
  return {".mat"};
}
