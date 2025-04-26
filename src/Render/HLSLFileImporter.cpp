#include <MyGE/Render/HLSLFile.h>
#include <MyGE/Render/HLSLFileImporter.h>

#include <filesystem>

using namespace Smkz::MyGE;

void HLSLFileImporter::RegisterToMyDRefl() { RegisterToMyDReflHelper(); }

AssetImportContext HLSLFileImporter::ImportAsset() const {
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

  std::shared_ptr<HLSLFile> hlsl =
      std::make_shared<HLSLFile>(str, path.parent_path().string());
  ctx.AddObject(name, MyDRefl::SharedObject{Type_of<HLSLFile>, hlsl});
  ctx.SetMainObjectID(name);

  return ctx;
}

std::vector<std::string> HLSLFileImporterCreator::SupportedExtentions() const {
  return {".hlsl"};
}
