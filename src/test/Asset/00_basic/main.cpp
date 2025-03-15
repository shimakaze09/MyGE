//
// Created by Admin on 15/03/2025.
//

#include <MyGE/Asset/AssetMngr.h>
#include <MyGE/ScriptSystem/LuaScript.h>

#include <iostream>

using namespace My::MyGE;

int main() {
  std::filesystem::path path = "../assets/scripts/test_00.lua";
  AssetMngr::Instance().ImportAsset(path);
  auto luaS = AssetMngr::Instance().LoadAsset<LuaScript>(path);
  std::cout << luaS->GetString() << std::endl;

  std::cout << AssetMngr::Instance().Contains(luaS) << std::endl;
  auto guid = AssetMngr::Instance().AssetPathToGUID(path);
  std::cout << guid.str() << std::endl;
  std::cout << AssetMngr::Instance().GUIDToAssetPath(guid).string()
            << std::endl;
  std::cout << AssetMngr::Instance().GetAssetPath(luaS).string() << std::endl;

  AssetMngr::Instance().Clear();
}
