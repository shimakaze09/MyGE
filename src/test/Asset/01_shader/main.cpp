#include <MyGE/Asset/AssetMngr.h>
#include <MyGE/Render/Shader.h>

#include <iostream>

using namespace My::MyGE;
using namespace My;

int main() {
  // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  AssetMngr::Instance().ImportAssetRecursively(L"..\\assets");

  auto shader =
      AssetMngr::Instance().LoadAsset<Shader>(L"..\\assets\\test\\test.shader");

  return 0;
}
