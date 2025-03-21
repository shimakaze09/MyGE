#pragma once

#include <MyDX12/UploadBuffer.h>

#include <unordered_map>

namespace My::MyGE {
struct Shader;

// every frame resource have a ShaderCBMngrDX12
// every shader have a self constant buffer
// TODO: share buffer
class ShaderCBMngrDX12 {
 public:
  ShaderCBMngrDX12(ID3D12Device* device) : device{device} {}
  ~ShaderCBMngrDX12();
  MyDX12::DynamicUploadBuffer* GetBuffer(const Shader* shader);
  MyDX12::DynamicUploadBuffer* GetCommonBuffer();

 private:
  std::unordered_map<size_t, MyDX12::DynamicUploadBuffer*> bufferMap;
  ID3D12Device* device;
};
}  // namespace My::MyGE
