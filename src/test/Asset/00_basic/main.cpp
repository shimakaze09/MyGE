//
// Created by Admin on 15/03/2025.
//

#include <MyGE/Asset/AssetMngr.h>
#include <MyGE/ScriptSystem/LuaScript.h>

#include <iostream>

using namespace My::MyGE;

int main() {
  AssetMngr::Instance().Init();
  AssetMngr::Instance().ImportAsset("../assets/scripts/test_00.lua");
  auto luaS = AssetMngr::Instance().LoadAssetAs<LuaScript>(
      "../assets/scripts/test_00.lua");
  std::cout << luaS->GetString() << std::endl;
  AssetMngr::Instance().Clear();
}
