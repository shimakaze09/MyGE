#include <MyGE/Render/DX12/ShaderCBMngrDX12.h>
#include <MyGE/Render/Shader.h>

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

My::MyDX12::DynamicUploadBuffer* ShaderCBMngrDX12::GetCommonBuffer() {
  size_t ID = static_cast<size_t>(-1);
  auto target = bufferMap.find(ID);
  if (target != bufferMap.end()) return target->second;

  auto rst = bufferMap.emplace_hint(target, ID,
                                    new MyDX12::DynamicUploadBuffer{device});
  return rst->second;
}
