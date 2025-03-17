//
// Created by Admin on 16/03/2025.
//

#pragma once

#include <MyDX12/MyDX12.h>

namespace My::MyECS {
class World;
}  // namespace My::MyECS

namespace My::MyGE {
class IPipeline {
 public:
  struct InitDesc {
    size_t numFrame;
    ID3D12Device* device;
    ID3D12CommandQueue* cmdQueue;
    DXGI_FORMAT rtFormat;
    DXGI_FORMAT depthStencilFormat;
  };

  IPipeline(InitDesc initDesc) : initDesc{initDesc} {}

  virtual ~IPipeline() = default;

  virtual void UpdateRenderContext(const MyECS::World& world) = 0;

  virtual void Render(ID3D12Resource* rt) = 0;
  virtual void EndFrame() = 0;

  void Resize(size_t width, size_t height, D3D12_VIEWPORT screenViewport,
              D3D12_RECT scissorRect, ID3D12Resource* depthStencilBuffer) {
    resizeData.width = width;
    resizeData.height = height;
    resizeData.screenViewport = screenViewport;
    resizeData.scissorRect = scissorRect;
    resizeData.depthStencilBuffer = depthStencilBuffer;
    Impl_Resize();
  }

 protected:
  virtual void Impl_Resize() = 0;

  struct ResizeData {
    size_t width{0};
    size_t height{0};
    D3D12_VIEWPORT screenViewport;
    D3D12_RECT scissorRect;
    ID3D12Resource* depthStencilBuffer{nullptr};
  };

  const InitDesc initDesc;

  const ResizeData& GetResizeData() const { return resizeData; }

 private:
  ResizeData resizeData;
};
}  // namespace My::MyGE
