#include <MyGE/Core/AssetMngr.h>
#include <MyGE/Render/HLSLFile.h>
#include <MyGE/Render/HLSLFileImporter.h>

#include <cassert>
#include <iostream>

using namespace Smkz;
using namespace Smkz::MyGE;

int main() {
  // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  std::filesystem::path root = LR"(..\src\test\x\03_HLSLImporter\assets)";
  AssetMngr::Instance().SetRootPath(root);

  {
    AssetMngr::Instance().RegisterAssetImporterCreator(
        std::make_shared<HLSLFileImporterCreator>());
    auto guid = AssetMngr::Instance().ImportAsset(LR"(hello.hlsl)");
    assert(guid.isValid());
    auto hlsl = AssetMngr::Instance().LoadAsset<HLSLFile>(LR"(hello.hlsl)");
    assert(hlsl.get());
    assert(hlsl->GetText() == "hello");
    assert(hlsl->GetLocalDir() == R"(..\src\test\x\03_HLSLImporter\assets)");
  }

  AssetMngr::Instance().Clear();
}
