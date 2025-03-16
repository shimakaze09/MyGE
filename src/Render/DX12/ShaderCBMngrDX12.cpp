//
// Created by Admin on 16/03/2025.
//

#include <MyGE/Core/Shader.h>
#include <MyGE/Render/DX12/ShaderCBMngrDX12.h>

using namespace My::MyGE;

ShaderCBMngrDX12::~ShaderCBMngrDX12() {
  for (auto [shaderID, buffer] : bufferMap) {
    delete buffer;
  }
}

My::MyDX12::DynamicUploadBuffer* ShaderCBMngrDX12::GetBuffer(
    const Shader* shader) {
  auto target = bufferMap.find(shader->GetInstanceID());
  if (target != bufferMap.end()) return target->second;

  auto rst = bufferMap.emplace_hint(target, shader->GetInstanceID(),
                                    new MyDX12::DynamicUploadBuffer{device});
  return rst->second;
}