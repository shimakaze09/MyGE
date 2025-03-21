//
// Created by Admin on 16/03/2025.
//

#pragma once

#include <MyDX12/MyDX12.h>

#include <MyECS/Entity.h>

#include <vector>

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
  };

  struct CameraData {
    CameraData(MyECS::Entity entity, const MyECS::World& world)
        : entity{entity}, world{world} {}

    MyECS::Entity entity;
    const MyECS::World& world;
  };

  IPipeline(InitDesc initDesc) : initDesc{initDesc} {}

  virtual ~IPipeline() = default;

  // data : cpu -> gpu
  // run in update
  virtual void BeginFrame(const std::vector<const MyECS::World*>& worlds,
                          const CameraData& cameraData) = 0;
  // run in draw
  virtual void Render(ID3D12Resource* rt) = 0;
  // run at the end of draw
  virtual void EndFrame() = 0;

  void Resize(size_t width, size_t height, D3D12_VIEWPORT screenViewport,
              D3D12_RECT scissorRect) {
    resizeData.width = width;
    resizeData.height = height;
    resizeData.screenViewport = screenViewport;
    resizeData.scissorRect = scissorRect;
    Impl_Resize();
  }

 protected:
  virtual void Impl_Resize() = 0;

  struct ResizeData {
    size_t width{0};
    size_t height{0};
    D3D12_VIEWPORT screenViewport;
    D3D12_RECT scissorRect;
  };

  const InitDesc initDesc;

  const ResizeData& GetResizeData() const { return resizeData; }

 private:
  ResizeData resizeData;
};
}  // namespace My::MyGE
