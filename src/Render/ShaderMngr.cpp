#include <MyGE/Render/Shader.h>
#include <MyGE/Render/ShaderMngr.h>

using namespace My::MyGE;

void ShaderMngr::Register(Shader* shader) {
  shaderMap[shader->shaderName] = shader;
}

Shader* ShaderMngr::Get(std::string_view name) const {
  auto target = shaderMap.find(name);
  if (target == shaderMap.end()) return nullptr;

  return target->second;
}
