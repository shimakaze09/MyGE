#include <MyGE/Render/ShaderImporter.h>
#include <MyGE/Render/Shader.h>

using namespace My::MyGE;

void ShaderImporter::RegisterToMyDRefl() {
  RegisterToMyDReflHelper();
  MyDRefl::Mngr.RegisterType<Shader>();
}

AssetImportContext ShaderImporter::ImportAsset() const {
  // TODO: Implement shader parsing
  return {};
}

std::vector<std::string> ShaderImporterCreator::SupportedExtentions() const {
  return {".shader"};
}
