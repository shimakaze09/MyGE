//
// Created by Admin on 16/03/2025.
//

#include <MyGE/Asset/AssetMngr.h>
#include <MyGE/ScriptSystem/LuaScript.h>

#include <iostream>

using namespace My::MyGE;

int main() {
  // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  AssetMngr::Instance().ImportAssetRecursively(L"..\\assets");
  auto dir = AssetMngr::Instance().LoadAsset(L"..\\assets\\shaders");
  std::cout << AssetMngr::Instance().GetAssetPath(dir) << std::endl;

  auto guids = AssetMngr::Instance().FindAssets(std::wregex{ LR"(.*\.shader)" });
  for (const auto& guid : guids) {
    std::cout << guid.str() << " : " << AssetMngr::Instance().GUIDToAssetPath(guid) << std::endl;
  }
  
  return 0;
}
