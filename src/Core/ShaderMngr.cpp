//
// Created by Admin on 17/03/2025.
//

#include <MyGE/Core/ShaderMngr.h>

#include <MyGE/Core/Shader.h>

using namespace My::MyGE;

void ShaderMngr::Register(Shader* shader) {
  shaderMap[shader->shaderName] = shader;
}

Shader* ShaderMngr::Get(std::string_view name) const {
  auto target = shaderMap.find(name);
  if (target == shaderMap.end())
    return nullptr;

  return target->second;
}
