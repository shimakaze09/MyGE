#pragma once

#include <MyDX12/MyDX12.h>
#include <MyECS/Entity.h>

#include <map>
#include <set>
#include <unordered_map>
#include <vector>

namespace My::MyECS {
class World;
}

namespace My::MyGE {
struct Material;
struct Shader;
class ShaderCBMngrDX12;
struct RenderState;

class PipelineBase {
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

  PipelineBase(InitDesc initDesc) : initDesc{initDesc} {}

  virtual ~PipelineBase() = default;

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

  struct ShaderCBDesc {
    // whole size == materialCBSize * globalOffsetMap.size()
    // offset = indexMap[material] * materialCBSize + offsetRef[register index]

    size_t materialCBSize{0};
    std::map<size_t, size_t> offsetMap;  // register index -> local offset
    std::unordered_map<const Material*, size_t> indexMap;
  };

  static ShaderCBDesc UpdateShaderCBs(
      ShaderCBMngrDX12& shaderCBMngr, const Shader* shader,
      const std::vector<const Material*>& materials,
      const std::set<std::string_view>& commonCBs);
  static void SetGraphicsRoot_CBV_SRV(
      ID3D12GraphicsCommandList* cmdList, ShaderCBMngrDX12& shaderCBMngr,
      const ShaderCBDesc& shaderCBDescconst, const Material* material,
      const std::map<std::string_view, D3D12_GPU_VIRTUAL_ADDRESS>& commonCBs,
      const std::map<std::string_view, D3D12_GPU_DESCRIPTOR_HANDLE>&
          commonSRVs);

  static void SetPSODescForRenderState(D3D12_GRAPHICS_PIPELINE_STATE_DESC&,
                                       const RenderState&);

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
