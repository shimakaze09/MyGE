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

  auto material = std::static_pointer_cast<Material>(Serializer::Instance().Deserialize(str).GetBuffer());

  if (material->shader) {
    for (const auto& [n, prop] : material->shader->properties) {
      auto target = material->properties.find(n);
      if (target != material->properties.end()) continue;

      material->properties.emplace_hint(target, n, prop);
    }
  }

  ctx.AddObject(name, MyDRefl::SharedObject{Type_of<Material>, material});
  ctx.SetMainObjectID(name);

  return ctx;
}

std::vector<std::string> MaterialImporterCreator::SupportedExtentions() const {
  return {".mat"};
}

