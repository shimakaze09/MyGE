//
// Created by Admin on 16/03/2025.
//

//***************************************************************************************
// WorldApp.cpp by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************

#include <MyDX12/UploadBuffer.h>
#include <MyGE/Asset/AssetMngr.h>
#include <MyGE/Core/Components/Camera.h>
#include <MyGE/Core/Components/MeshFilter.h>
#include <MyGE/Core/Components/MeshRenderer.h>
#include <MyGE/Core/HLSLFile.h>
#include <MyGE/Core/Image.h>
#include <MyGE/Core/Mesh.h>
#include <MyGE/Core/Shader.h>
#include <MyGE/Core/Systems/CameraSystem.h>
#include <MyGE/Core/Texture2D.h>
#include <MyGE/Render/DX12/ShaderCBMngrDX12.h>
#include <MyGE/Transform/Transform.h>
#include <MyGM/MyGM.h>

#include "../common/GeometryGenerator.h"
#include "../common/MathHelper.h"
#include "../common/d3dApp.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

const int gNumFrameResources = 3;

constexpr size_t ID_PSO_geometry = 0;
constexpr size_t ID_PSO_defer_light = 1;
constexpr size_t ID_PSO_screen = 2;

constexpr size_t ID_RootSignature_geometry = 0;
constexpr size_t ID_RootSignature_screen = 1;
constexpr size_t ID_RootSignature_defer_light = 2;

struct ObjectConstants {
  My::transformf World = My::transformf::eye();
  My::transformf TexTransform = My::transformf::eye();
};

struct PassConstants {
  My::transformf View = My::transformf::eye();
  My::transformf InvView = My::transformf::eye();
  My::transformf Proj = My::transformf::eye();
  My::transformf InvProj = My::transformf::eye();
  My::transformf ViewProj = My::transformf::eye();
  My::transformf InvViewProj = My::transformf::eye();
  My::pointf3 EyePosW = {0.0f, 0.0f, 0.0f};
  float cbPerObjectPad1 = 0.0f;
  My::valf2 RenderTargetSize = {0.0f, 0.0f};
  My::valf2 InvRenderTargetSize = {0.0f, 0.0f};
  float NearZ = 0.0f;
  float FarZ = 0.0f;
  float TotalTime = 0.0f;
  float DeltaTime = 0.0f;

  My::vecf4 AmbientLight = {0.0f, 0.0f, 0.0f, 1.0f};

  // Indices [0, NUM_DIR_LIGHTS) are directional lights;
  // indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
  // indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS,
  // NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS) are spot lights for a
  // maximum of MaxLights per object.
  Light Lights[MaxLights];
};

struct MatConstants {
  My::rgbf albedoFactor;
  float roughnessFactor;
};

struct RenderContext {
  std::vector<My::MyECS::Entity> cameras;

  struct Object {
    My::MyECS::Entity entity;
    size_t submeshIdx;
  };
  std::unordered_map<
      const My::MyGE::Shader*,
      std::unordered_map<const My::MyGE::Material*, std::vector<Object>>>
      objectMap;
};

struct Vertex {
  My::pointf3 Pos;
  My::normalf Normal;
  My::pointf2 TexC;
};

// Lightweight structure stores parameters to draw a shape.  This will
// vary from app-to-app.
struct RenderItem {
  RenderItem() = default;

  // World matrix of the shape that describes the object's local space
  // relative to the world space, which defines the position, orientation,
  // and scale of the object in the world.
  My::transformf World = My::transformf::eye();

  My::transformf TexTransform = My::transformf::eye();

  // Dirty flag indicating the object data has changed and we need to update the
  // constant buffer. Because we have an object cbuffer for each FrameResource,
  // we have to apply the update to each FrameResource.  Thus, when we modify
  // obect data we should set NumFramesDirty = gNumFrameResources so that each
  // frame resource gets the update.
  int NumFramesDirty = gNumFrameResources;

  // Index into GPU constant buffer corresponding to the ObjectCB for this
  // render item.
  UINT ObjCBIndex = -1;

  Material* Mat = nullptr;
  My::MyDX12::MeshGPUBuffer* Geo = nullptr;
  // std::string Geo;

  // Primitive topology.
  D3D12_PRIMITIVE_TOPOLOGY PrimitiveType =
      D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

  // DrawIndexedInstanced parameters.
  UINT IndexCount = 0;
  UINT StartIndexLocation = 0;
  int BaseVertexLocation = 0;
};

struct RotateSystem : My::MyECS::System {
  using My::MyECS::System::System;
  virtual void OnUpdate(My::MyECS::Schedule& schedule) override {
    My::MyECS::ArchetypeFilter filter;
    filter.all = {My::MyECS::CmptType::Of<My::MyGE::MeshFilter>};
    schedule.RegisterEntityJob(
        [](My::MyGE::Rotation* rot) {
          rot->value =
              rot->value * My::quatf{My::vecf3{0, 1, 0}, My::to_radian(2.f)};
        },
        "rotate", filter);
  }
};

class WorldApp : public D3DApp {
 public:
  WorldApp(HINSTANCE hInstance);
  WorldApp(const WorldApp& rhs) = delete;
  WorldApp& operator=(const WorldApp& rhs) = delete;
  ~WorldApp();

  virtual bool Initialize() override;

 private:
  virtual void OnResize() override;
  virtual void Update(const GameTimer& gt) override;
  virtual void Draw(const GameTimer& gt) override;

  virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
  virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
  virtual void OnMouseMove(WPARAM btnState, int x, int y) override;

  void OnKeyboardInput(const GameTimer& gt);
  void UpdateCamera(const GameTimer& gt);
  void AnimateMaterials(const GameTimer& gt);
  void UpdateObjectCBs(const GameTimer& gt);
  void UpdateMaterialCBs(const GameTimer& gt);
  void UpdateMainPassCB(const GameTimer& gt);

  void LoadTextures();
  void BuildRootSignature();
  void BuildDescriptorHeaps();
  void BuildShadersAndInputLayout();
  void BuildShapeGeometry();
  void BuildPSOs();
  void BuildFrameResources();
  void BuildMaterials();
  void BuildRenderItems();
  void DrawRenderItems(ID3D12GraphicsCommandList* cmdList);

 private:
  std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;

  std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

  // List of all the render items.
  std::vector<std::unique_ptr<RenderItem>> mAllRitems;

  // Render items divided by PSO.
  std::vector<RenderItem*> mOpaqueRitems;

  PassConstants mMainPassCB;

  // My::pointf3 mEyePos = { 0.0f, 0.0f, 0.0f };
  // My::transformf mView = My::transformf::eye();
  // My::transformf mProj = My::transformf::eye();

  float mTheta = 0.4f * XM_PI;
  float mPhi = 1.3f * XM_PI;
  float mRadius = 5.0f;

  POINT mLastMousePos;

  // frame graph
  // My::MyDX12::FG::RsrcMngr fgRsrcMngr;
  My::MyDX12::FG::Executor fgExecutor;
  My::MyFG::Compiler fgCompiler;
  My::MyFG::FrameGraph fg;

  My::MyGE::Shader* screenShader;
  My::MyGE::Shader* geomrtryShader;
  My::MyGE::Shader* deferShader;

  My::MyGE::Texture2D* albedoTex2D;
  My::MyGE::Texture2D* roughnessTex2D;
  My::MyGE::Texture2D* metalnessTex2D;

  My::MyECS::World world;
  My::MyECS::Entity cam{My::MyECS::Entity::Invalid()};
  float fov{0.33f * My::PI<float>};
  float aspect{16.f / 9.f};

  std::unique_ptr<My::MyDX12::FrameResourceMngr> frameRsrcMngr;

  RenderContext renderContext;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine,
                   int showCmd) {
  // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  try {
    WorldApp theApp(hInstance);
    if (!theApp.Initialize()) return 0;

    int rst = theApp.Run();
    My::MyGE::RsrcMngrDX12::Instance().Clear();
    return rst;
  } catch (My::MyDX12::Util::Exception& e) {
    MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
    My::MyGE::RsrcMngrDX12::Instance().Clear();
    return 0;
  }
}

WorldApp::WorldApp(HINSTANCE hInstance)
    : D3DApp(hInstance), fg{"frame graph"} {}

WorldApp::~WorldApp() {
  if (!uDevice.IsNull()) FlushCommandQueue();
}

bool WorldApp::Initialize() {
  if (!D3DApp::Initialize()) return false;

  frameRsrcMngr = std::make_unique<My::MyDX12::FrameResourceMngr>(
      gNumFrameResources, uDevice.raw.Get());

  My::MyGE::RsrcMngrDX12::Instance().Init(uDevice.raw.Get());

  My::MyDX12::DescriptorHeapMngr::Instance().Init(uDevice.raw.Get(), 1024, 1024,
                                                  1024, 1024, 1024);

  world.systemMngr.Register<
      My::MyGE::CameraSystem, My::MyGE::LocalToParentSystem,
      My::MyGE::RotationEulerSystem, My::MyGE::TRSToLocalToParentSystem,
      My::MyGE::TRSToLocalToWorldSystem, My::MyGE::WorldToLocalSystem,
      RotateSystem>();

  auto e0 =
      world.entityMngr.Create<My::MyGE::LocalToWorld, My::MyGE::WorldToLocal,
                              My::MyGE::Camera, My::MyGE::Translation,
                              My::MyGE::Rotation>();
  cam = std::get<My::MyECS::Entity>(e0);

  int num = 11;
  for (int i = 0; i < num; i++) {
    for (int j = 0; j < num; j++) {
      auto cube =
          world.entityMngr.Create<My::MyGE::LocalToWorld, My::MyGE::MeshFilter,
                                  My::MyGE::MeshRenderer, My::MyGE::Translation,
                                  My::MyGE::Rotation, My::MyGE::Scale>();
      auto t = std::get<My::MyGE::Translation*>(cube);
      auto s = std::get<My::MyGE::Scale*>(cube);
      s->value = 0.2f;
      t->value = {0.5f * (i - num / 2), 0.5f * (j - num / 2), 0};
    }
  }

  // fgRsrcMngr.Init(uGCmdList, uDevice);

  // Reset the command list to prep for initialization commands.
  ThrowIfFailed(uGCmdList->Reset(mDirectCmdListAlloc.Get(), nullptr));

  // Get the increment size of a descriptor in this heap type.  This is hardware
  // specific, so we have to query this information.
  // mCbvSrvDescriptorSize =
  // uDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

  My::MyGE::RsrcMngrDX12::Instance().GetUpload().Begin();

  LoadTextures();
  BuildRootSignature();
  BuildDescriptorHeaps();
  BuildShadersAndInputLayout();
  BuildShapeGeometry();
  BuildMaterials();
  BuildRenderItems();
  BuildFrameResources();
  BuildPSOs();

  // Execute the initialization commands.
  ThrowIfFailed(uGCmdList->Close());
  uCmdQueue.Execute(uGCmdList.raw.Get());

  My::MyGE::RsrcMngrDX12::Instance().GetUpload().End(uCmdQueue.raw.Get());

  // Wait until initialization is complete.
  FlushCommandQueue();

  return true;
}

void WorldApp::OnResize() {
  D3DApp::OnResize();

  // The window resized, so update the aspect ratio and recompute the projection
  // matrix.
  auto clearFGRsrcMngr =
      [](std::shared_ptr<My::MyDX12::FG::RsrcMngr> rsrcMngr) {
        rsrcMngr->Clear();
      };

  if (frameRsrcMngr) {
    for (auto& frsrc : frameRsrcMngr->GetFrameResources())
      frsrc->DelayUpdateResource("FrameGraphRsrcMngr", clearFGRsrcMngr);
  }
}

void WorldApp::Update(const GameTimer& gt) {
  OnKeyboardInput(gt);
  UpdateCamera(gt);

  world.Update();

  // Cycle through the circular frame resource array.
  // Has the GPU finished processing the commands of the current frame resource?
  // If not, wait until the GPU has completed commands up to this fence point.
  frameRsrcMngr->BeginFrame();

  // AnimateMaterials(gt);
  // UpdateObjectCBs(gt);
  // UpdateMaterialCBs(gt);
  UpdateMainPassCB(gt);

  renderContext.cameras.clear();
  renderContext.objectMap.clear();

  My::MyECS::ArchetypeFilter objectFilter;
  objectFilter.all = {My::MyECS::CmptType::Of<My::MyGE::MeshFilter>,
                      My::MyECS::CmptType::Of<My::MyGE::MeshRenderer>};
  auto objectEntities = world.entityMngr.GetEntityArray(objectFilter);
  for (auto e : objectEntities) {
    auto meshFilter = world.entityMngr.Get<My::MyGE::MeshFilter>(e);
    auto meshRenderer = world.entityMngr.Get<My::MyGE::MeshRenderer>(e);
    for (size_t i = 0; i < meshRenderer->material.size(); i++) {
      auto mat = meshRenderer->material[i];
      renderContext.objectMap[mat->shader][mat].push_back(
          RenderContext::Object{e, i});
    }
  }

  My::MyECS::ArchetypeFilter cameraFilter;
  cameraFilter.all = {My::MyECS::CmptType::Of<My::MyGE::Camera>};
  renderContext.cameras = world.entityMngr.GetEntityArray(cameraFilter);

  auto& shaderCBMngr =
      frameRsrcMngr->GetCurrentFrameResource()
          ->GetResource<My::MyGE::ShaderCBMngrDX12>("ShaderCBMngrDX12");

  for (const auto& [shader, mat2objects] : renderContext.objectMap) {
    size_t objectNum = 0;
    for (const auto& [mat, objects] : mat2objects) objectNum += objects.size();
    if (shader->shaderName == "Geometry") {
      assert(renderContext.cameras.size() == 1);
      auto buffer = shaderCBMngr.GetBuffer(shader);
      buffer->Reserve(renderContext.cameras.size() *
                          My::MyDX12::Util::CalcConstantBufferByteSize(
                              sizeof(PassConstants)) +
                      mat2objects.size() *
                          My::MyDX12::Util::CalcConstantBufferByteSize(
                              sizeof(MatConstants)) +
                      objectNum * My::MyDX12::Util::CalcConstantBufferByteSize(
                                      sizeof(ObjectConstants)));
      size_t offset = 0;
      assert(renderContext.cameras.size() == 1);
      buffer->Set(0, &mMainPassCB, sizeof(PassConstants));
      offset +=
          My::MyDX12::Util::CalcConstantBufferByteSize(sizeof(PassConstants));
      for (const auto& [mat, objects] : mat2objects) {
        MatConstants matC;
        matC.albedoFactor = {1.f};
        matC.roughnessFactor = 1.f;
        buffer->Set(offset, &matC, sizeof(MatConstants));
        offset +=
            My::MyDX12::Util::CalcConstantBufferByteSize(sizeof(MatConstants));
        for (const auto& object : objects) {
          ObjectConstants objectConstants;
          objectConstants.TexTransform = My::transformf::eye();
          auto l2w =
              world.entityMngr.Get<My::MyGE::LocalToWorld>(object.entity);
          objectConstants.World = l2w->value;
          buffer->Set(offset, &objectConstants, sizeof(ObjectConstants));
          offset += My::MyDX12::Util::CalcConstantBufferByteSize(
              sizeof(ObjectConstants));
        }
      }
    }
  }
}

void WorldApp::Draw(const GameTimer& gt) {
  auto cmdListAlloc =
      frameRsrcMngr->GetCurrentFrameResource()
          ->GetResource<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>>(
              "CommandAllocator");

  // Reuse the memory associated with command recording.
  // We can only reset when the associated command lists have finished execution
  // on the GPU.
  ThrowIfFailed(cmdListAlloc->Reset());

  // A command list can be reset after it has been added to the command queue
  // via ExecuteCommandList. Reusing the command list reuses memory.
  ThrowIfFailed(uGCmdList->Reset(cmdListAlloc.Get(), nullptr));
  uGCmdList.SetDescriptorHeaps(My::MyDX12::DescriptorHeapMngr::Instance()
                                   .GetCSUGpuDH()
                                   ->GetDescriptorHeap());

  uGCmdList->RSSetViewports(1, &mScreenViewport);
  uGCmdList->RSSetScissorRects(1, &mScissorRect);

  fg.Clear();
  auto fgRsrcMngr =
      frameRsrcMngr->GetCurrentFrameResource()
          ->GetResource<std::shared_ptr<My::MyDX12::FG::RsrcMngr>>(
              "FrameGraphRsrcMngr");
  fgRsrcMngr->NewFrame();
  fgExecutor.NewFrame();
  ;

  auto gbuffer0 = fg.RegisterResourceNode("GBuffer0");
  auto gbuffer1 = fg.RegisterResourceNode("GBuffer1");
  auto gbuffer2 = fg.RegisterResourceNode("GBuffer2");
  auto backbuffer = fg.RegisterResourceNode("Back Buffer");
  auto depthstencil = fg.RegisterResourceNode("Depth Stencil");
  auto gbPass = fg.RegisterPassNode(
      "GBuffer Pass", {}, {gbuffer0, gbuffer1, gbuffer2, depthstencil});
  /*auto debugPass = fg.AddPassNode(
          "Debug",
          { gbuffer1 },
          { backbuffer }
  );*/
  auto deferLightingPass = fg.RegisterPassNode(
      "Defer Lighting", {gbuffer0, gbuffer1, gbuffer2}, {backbuffer});

  (*fgRsrcMngr)
      .RegisterTemporalRsrc(gbuffer0,
                            My::MyDX12::FG::RsrcType::RT2D(
                                DXGI_FORMAT_R32G32B32A32_FLOAT, mClientWidth,
                                mClientHeight, Colors::Black))
      .RegisterTemporalRsrc(gbuffer1,
                            My::MyDX12::FG::RsrcType::RT2D(
                                DXGI_FORMAT_R32G32B32A32_FLOAT, mClientWidth,
                                mClientHeight, Colors::Black))
      .RegisterTemporalRsrc(gbuffer2,
                            My::MyDX12::FG::RsrcType::RT2D(
                                DXGI_FORMAT_R32G32B32A32_FLOAT, mClientWidth,
                                mClientHeight, Colors::Black))

      .RegisterRsrcTable(
          {{gbuffer0,
            My::MyDX12::Desc::SRV::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT)},
           {gbuffer1,
            My::MyDX12::Desc::SRV::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT)},
           {gbuffer2,
            My::MyDX12::Desc::SRV::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT)}})

      .RegisterImportedRsrc(backbuffer,
                            {CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT})
      .RegisterImportedRsrc(depthstencil, {mDepthStencilBuffer.Get(),
                                           D3D12_RESOURCE_STATE_DEPTH_WRITE})

      .RegisterPassRsrcs(gbPass, gbuffer0, D3D12_RESOURCE_STATE_RENDER_TARGET,
                         My::MyDX12::FG::RsrcImplDesc_RTV_Null{})
      .RegisterPassRsrcs(gbPass, gbuffer1, D3D12_RESOURCE_STATE_RENDER_TARGET,
                         My::MyDX12::FG::RsrcImplDesc_RTV_Null{})
      .RegisterPassRsrcs(gbPass, gbuffer2, D3D12_RESOURCE_STATE_RENDER_TARGET,
                         My::MyDX12::FG::RsrcImplDesc_RTV_Null{})
      .RegisterPassRsrcs(gbPass, depthstencil, D3D12_RESOURCE_STATE_DEPTH_WRITE,
                         My::MyDX12::Desc::DSV::Basic(mDepthStencilFormat))

      /*.RegisterPassRsrcs(debugPass, gbuffer1,
      D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
              My::MyDX12::Desc::SRV::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT))

      .RegisterPassRsrcs(debugPass, backbuffer,
      D3D12_RESOURCE_STATE_RENDER_TARGET,
              My::MyDX12::FG::RsrcImplDesc_RTV_Null{})*/

      .RegisterPassRsrcs(
          deferLightingPass, gbuffer0,
          D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
          My::MyDX12::Desc::SRV::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT))
      .RegisterPassRsrcs(
          deferLightingPass, gbuffer1,
          D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
          My::MyDX12::Desc::SRV::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT))
      .RegisterPassRsrcs(
          deferLightingPass, gbuffer2,
          D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
          My::MyDX12::Desc::SRV::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT))

      .RegisterPassRsrcs(deferLightingPass, backbuffer,
                         D3D12_RESOURCE_STATE_RENDER_TARGET,
                         My::MyDX12::FG::RsrcImplDesc_RTV_Null{});

  fgExecutor.RegisterPassFunc(
      gbPass, [&](const My::MyDX12::FG::PassRsrcs& rsrcs) {
        uGCmdList->SetPipelineState(
            My::MyGE::RsrcMngrDX12::Instance().GetPSO(ID_PSO_geometry));
        auto gb0 = rsrcs.find(gbuffer0)->second;
        auto gb1 = rsrcs.find(gbuffer1)->second;
        auto gb2 = rsrcs.find(gbuffer2)->second;
        auto ds = rsrcs.find(depthstencil)->second;

        // Clear the render texture and depth buffer.
        uGCmdList.ClearRenderTargetView(gb0.cpuHandle, Colors::Black);
        uGCmdList.ClearRenderTargetView(gb1.cpuHandle, Colors::Black);
        uGCmdList.ClearRenderTargetView(gb2.cpuHandle, Colors::Black);
        uGCmdList.ClearDepthStencilView(ds.cpuHandle);

        // Specify the buffers we are going to render to.
        std::array rts{gb0.cpuHandle, gb1.cpuHandle, gb2.cpuHandle};
        uGCmdList->OMSetRenderTargets(rts.size(), rts.data(), false,
                                      &ds.cpuHandle);

        uGCmdList->SetGraphicsRootSignature(
            My::MyGE::RsrcMngrDX12::Instance().GetRootSignature(
                ID_RootSignature_geometry));

        auto passCB =
            frameRsrcMngr->GetCurrentFrameResource()
                ->GetResource<My::MyDX12::ArrayUploadBuffer<PassConstants>>(
                    "gbPass constants")
                .GetResource();
        uGCmdList->SetGraphicsRootConstantBufferView(
            4, passCB->GetGPUVirtualAddress());

        DrawRenderItems(uGCmdList.raw.Get());
      });

  // fgExecutor.RegisterPassFunc(
  //	debugPass,
  //	[&](const My::MyDX12::FG::PassRsrcs& rsrcs) {
  //		uGCmdList->SetPipelineState(My::MyGE::RsrcMngrDX12::Instance().GetPSO(ID_PSO_screen));
  //		auto img = rsrcs.find(gbuffer1)->second;
  //		auto bb = rsrcs.find(backbuffer)->second;
  //
  //		//uGCmdList->CopyResource(bb.resource, rt.resource);

  //		// Clear the render texture and depth buffer.
  //		uGCmdList.ClearRenderTargetView(bb.cpuHandle,
  // Colors::LightSteelBlue);

  //		// Specify the buffers we are going to render to.
  //		//uGCmdList.OMSetRenderTarget(bb.cpuHandle, ds.cpuHandle);
  //		uGCmdList->OMSetRenderTargets(1, &bb.cpuHandle, false, nullptr);

  //		uGCmdList->SetGraphicsRootSignature(My::MyGE::RsrcMngrDX12::Instance().GetRootSignature(ID_RootSignature_screen));

  //		uGCmdList->SetGraphicsRootDescriptorTable(0, img.gpuHandle);

  //		uGCmdList->IASetVertexBuffers(0, 0, nullptr);
  //		uGCmdList->IASetIndexBuffer(nullptr);
  //		uGCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  //		uGCmdList->DrawInstanced(6, 1, 0, 0);
  //	}
  //);

  fgExecutor.RegisterPassFunc(
      deferLightingPass, [&](const My::MyDX12::FG::PassRsrcs& rsrcs) {
        uGCmdList->SetPipelineState(
            My::MyGE::RsrcMngrDX12::Instance().GetPSO(ID_PSO_defer_light));
        auto gb0 = rsrcs.find(gbuffer0)->second;
        auto gb1 = rsrcs.find(gbuffer1)->second;
        auto gb2 = rsrcs.find(gbuffer2)->second;

        auto bb = rsrcs.find(backbuffer)->second;

        // uGCmdList->CopyResource(bb.resource, rt.resource);

        // Clear the render texture and depth buffer.
        uGCmdList.ClearRenderTargetView(bb.cpuHandle, Colors::LightSteelBlue);

        // Specify the buffers we are going to render to.
        // uGCmdList.OMSetRenderTarget(bb.cpuHandle, ds.cpuHandle);
        uGCmdList->OMSetRenderTargets(1, &bb.cpuHandle, false, nullptr);

        uGCmdList->SetGraphicsRootSignature(
            My::MyGE::RsrcMngrDX12::Instance().GetRootSignature(
                ID_RootSignature_defer_light));

        uGCmdList->SetGraphicsRootDescriptorTable(0, gb0.gpuHandle);

        uGCmdList->IASetVertexBuffers(0, 0, nullptr);
        uGCmdList->IASetIndexBuffer(nullptr);
        uGCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        uGCmdList->DrawInstanced(6, 1, 0, 0);
      });

  static bool flag{false};
  if (!flag) {
    OutputDebugStringA(fg.ToGraphvizGraph().Dump().c_str());
    flag = true;
  }

  auto [success, crst] = fgCompiler.Compile(fg);
  fgExecutor.Execute(crst, *fgRsrcMngr);

  // Done recording commands.
  ThrowIfFailed(uGCmdList->Close());

  // Add the command list to the queue for execution.
  uCmdQueue.Execute(uGCmdList.raw.Get());

  // Swap the back and front buffers
  ThrowIfFailed(mSwapChain->Present(0, 0));
  mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

  frameRsrcMngr->EndFrame(uCmdQueue.raw.Get());
}

void WorldApp::OnMouseDown(WPARAM btnState, int x, int y) {
  mLastMousePos.x = x;
  mLastMousePos.y = y;

  SetCapture(mhMainWnd);
}

void WorldApp::OnMouseUp(WPARAM btnState, int x, int y) { ReleaseCapture(); }

void WorldApp::OnMouseMove(WPARAM btnState, int x, int y) {
  if ((btnState & MK_LBUTTON) != 0) {
    // Make each pixel correspond to a quarter of a degree.
    float dx =
        XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
    float dy =
        XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

    // Update angles based on input to orbit camera around box.
    mTheta -= dy;
    mPhi -= dx;

    // Restrict the angle mPhi.
    mTheta = MathHelper::Clamp(mTheta, 0.1f, MathHelper::Pi - 0.1f);
  } else if ((btnState & MK_RBUTTON) != 0) {
    // Make each pixel correspond to 0.2 unit in the scene.
    float dx = 0.05f * static_cast<float>(x - mLastMousePos.x);
    float dy = 0.05f * static_cast<float>(y - mLastMousePos.y);

    // Update the camera radius based on input.
    mRadius += dx - dy;

    // Restrict the radius.
    mRadius = MathHelper::Clamp(mRadius, 5.0f, 150.0f);
  }

  mLastMousePos.x = x;
  mLastMousePos.y = y;
}

void WorldApp::OnKeyboardInput(const GameTimer& gt) {}

void WorldApp::UpdateCamera(const GameTimer& gt) {
  My::vecf3 eye = {mRadius * sinf(mTheta) * sinf(mPhi), mRadius * cosf(mTheta),
                   mRadius * sinf(mTheta) * cosf(mPhi)};
  auto camera = world.entityMngr.Get<My::MyGE::Camera>(cam);
  camera->fov = 0.25f * MathHelper::Pi;
  camera->aspect = AspectRatio();
  camera->clippingPlaneMin = 1.0f;
  camera->clippingPlaneMax = 1000.0f;
  auto view =
      My::transformf::look_at(eye.as<My::pointf3>(), {0.f});  // world to camera
  auto c2w = view.inverse();
  world.entityMngr.Get<My::MyGE::Translation>(cam)->value = eye;
  world.entityMngr.Get<My::MyGE::Rotation>(cam)->value =
      c2w.decompose_quatenion();
}

void WorldApp::AnimateMaterials(const GameTimer& gt) {}

void WorldApp::UpdateObjectCBs(const GameTimer& gt) {
  auto& currObjectCB =
      frameRsrcMngr->GetCurrentFrameResource()
          ->GetResource<My::MyDX12::ArrayUploadBuffer<ObjectConstants>>(
              "ArrayUploadBuffer<ObjectConstants>");
  for (auto& e : mAllRitems) {
    // Only update the cbuffer data if the constants have changed.
    // This needs to be tracked per frame resource.
    if (e->NumFramesDirty > 0) {
      ObjectConstants objConstants;
      objConstants.World = e->World;
      objConstants.TexTransform = e->TexTransform;

      currObjectCB.Set(e->ObjCBIndex, objConstants);

      // Next FrameResource need to be updated too.
      e->NumFramesDirty--;
    }
  }
}

void WorldApp::UpdateMaterialCBs(const GameTimer& gt) {
  auto& currMaterialCB =
      frameRsrcMngr->GetCurrentFrameResource()
          ->GetResource<My::MyDX12::ArrayUploadBuffer<MaterialConstants>>(
              "ArrayUploadBuffer<MaterialConstants>");
  for (auto& e : mMaterials) {
    // Only update the cbuffer data if the constants have changed.  If the
    // cbuffer data changes, it needs to be updated for each FrameResource.
    Material* mat = e.second.get();
    if (mat->NumFramesDirty > 0) {
      XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);

      MaterialConstants matConstants;
      matConstants.DiffuseAlbedo = mat->DiffuseAlbedo;
      matConstants.FresnelR0 = mat->FresnelR0;
      matConstants.Roughness = mat->Roughness;
      XMStoreFloat4x4(&matConstants.MatTransform,
                      XMMatrixTranspose(matTransform));

      currMaterialCB.Set(mat->MatCBIndex, matConstants);

      // Next FrameResource need to be updated too.
      mat->NumFramesDirty--;
    }
  }
}

void WorldApp::UpdateMainPassCB(const GameTimer& gt) {
  auto camera = world.entityMngr.Get<My::MyGE::Camera>(cam);
  auto view = world.entityMngr.Get<My::MyGE::WorldToLocal>(cam);
  auto pos = world.entityMngr.Get<My::MyGE::Translation>(cam);
  mMainPassCB.View = view->value;
  mMainPassCB.InvView = mMainPassCB.View.inverse();
  mMainPassCB.Proj = camera->prjectionMatrix;
  mMainPassCB.InvProj = mMainPassCB.Proj.inverse();
  mMainPassCB.ViewProj = mMainPassCB.Proj * mMainPassCB.View;
  mMainPassCB.InvViewProj = mMainPassCB.InvView * mMainPassCB.InvProj;
  mMainPassCB.EyePosW = pos->value.as<My::pointf3>();
  mMainPassCB.RenderTargetSize = {mClientWidth, mClientHeight};
  mMainPassCB.InvRenderTargetSize = {1.0f / mClientWidth, 1.0f / mClientHeight};

  mMainPassCB.NearZ = 1.0f;
  mMainPassCB.FarZ = 1000.0f;
  mMainPassCB.TotalTime = gt.TotalTime();
  mMainPassCB.DeltaTime = gt.DeltaTime();
  mMainPassCB.AmbientLight = {0.25f, 0.25f, 0.35f, 1.0f};
  mMainPassCB.Lights[0].Direction = {0.57735f, -0.57735f, 0.57735f};
  mMainPassCB.Lights[0].Strength = {0.6f, 0.6f, 0.6f};
  mMainPassCB.Lights[1].Direction = {-0.57735f, -0.57735f, 0.57735f};
  mMainPassCB.Lights[1].Strength = {0.3f, 0.3f, 0.3f};
  mMainPassCB.Lights[2].Direction = {0.0f, -0.707f, -0.707f};
  mMainPassCB.Lights[2].Strength = {0.15f, 0.15f, 0.15f};

  auto& currPassCB =
      frameRsrcMngr->GetCurrentFrameResource()
          ->GetResource<My::MyDX12::ArrayUploadBuffer<PassConstants>>(
              "gbPass constants");
  currPassCB.Set(0, mMainPassCB);
}

void WorldApp::LoadTextures() {
  auto albedoImg = My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Image>(
      "../assets/textures/iron/albedo.png");
  auto roughnessImg =
      My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Image>(
          "../assets/textures/iron/roughness.png");
  auto metalnessImg =
      My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Image>(
          "../assets/textures/iron/metalness.png");

  albedoTex2D = new My::MyGE::Texture2D;
  albedoTex2D->image = albedoImg;
  if (!My::MyGE::AssetMngr::Instance().CreateAsset(
          albedoTex2D, "../assets/textures/iron/albedo.tex2d")) {
    delete albedoTex2D;
    albedoTex2D =
        My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Texture2D>(
            "../assets/textures/iron/albedo.tex2d");
  }

  roughnessTex2D = new My::MyGE::Texture2D;
  roughnessTex2D->image = roughnessImg;
  if (!My::MyGE::AssetMngr::Instance().CreateAsset(
          roughnessTex2D, "../assets/textures/iron/roughness.tex2d")) {
    delete roughnessTex2D;
    roughnessTex2D =
        My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Texture2D>(
            "../assets/textures/iron/roughness.tex2d");
  }

  metalnessTex2D = new My::MyGE::Texture2D;
  metalnessTex2D->image = metalnessImg;
  if (!My::MyGE::AssetMngr::Instance().CreateAsset(
          metalnessTex2D, "../assets/textures/iron/metalness.tex2d")) {
    delete metalnessTex2D;
    metalnessTex2D =
        My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Texture2D>(
            "../assets/textures/iron/metalness.tex2d");
  }

  My::MyGE::RsrcMngrDX12::Instance().RegisterTexture2D(
      My::MyGE::RsrcMngrDX12::Instance().GetUpload(), albedoTex2D);
  My::MyGE::RsrcMngrDX12::Instance().RegisterTexture2D(
      My::MyGE::RsrcMngrDX12::Instance().GetUpload(), roughnessTex2D);
  My::MyGE::RsrcMngrDX12::Instance().RegisterTexture2D(
      My::MyGE::RsrcMngrDX12::Instance().GetUpload(), metalnessTex2D);
}

void WorldApp::BuildRootSignature() {
  {  // geometry
    CD3DX12_DESCRIPTOR_RANGE texRange0;
    texRange0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
    CD3DX12_DESCRIPTOR_RANGE texRange1;
    texRange1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
    CD3DX12_DESCRIPTOR_RANGE texRange2;
    texRange2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);

    // Root parameter can be a table, root descriptor or root constants.
    CD3DX12_ROOT_PARAMETER slotRootParameter[6];

    // Perfomance TIP: Order from most frequent to least frequent.
    slotRootParameter[0].InitAsDescriptorTable(1, &texRange0);
    slotRootParameter[1].InitAsDescriptorTable(1, &texRange1);
    slotRootParameter[2].InitAsDescriptorTable(1, &texRange2);
    slotRootParameter[3].InitAsConstantBufferView(0);
    slotRootParameter[4].InitAsConstantBufferView(1);
    slotRootParameter[5].InitAsConstantBufferView(2);

    auto staticSamplers =
        My::MyGE::RsrcMngrDX12::Instance().GetStaticSamplers();

    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
        6, slotRootParameter, (UINT)staticSamplers.size(),
        staticSamplers.data(),
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    My::MyGE::RsrcMngrDX12::Instance().RegisterRootSignature(
        ID_RootSignature_geometry, &rootSigDesc);
  }

  {  // screen
    CD3DX12_DESCRIPTOR_RANGE texTable;
    texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

    // Root parameter can be a table, root descriptor or root constants.
    CD3DX12_ROOT_PARAMETER slotRootParameter[1];

    // Perfomance TIP: Order from most frequent to least frequent.
    slotRootParameter[0].InitAsDescriptorTable(1, &texTable,
                                               D3D12_SHADER_VISIBILITY_PIXEL);

    auto staticSamplers =
        My::MyGE::RsrcMngrDX12::Instance().GetStaticSamplers();

    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
        1, slotRootParameter, (UINT)staticSamplers.size(),
        staticSamplers.data(),
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    My::MyGE::RsrcMngrDX12::Instance().RegisterRootSignature(
        ID_RootSignature_screen, &rootSigDesc);
  }
  {  // defer lighting
    CD3DX12_DESCRIPTOR_RANGE texTable;
    texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0);

    // Root parameter can be a table, root descriptor or root constants.
    CD3DX12_ROOT_PARAMETER slotRootParameter[4];

    // Perfomance TIP: Order from most frequent to least frequent.
    slotRootParameter[0].InitAsDescriptorTable(1, &texTable,
                                               D3D12_SHADER_VISIBILITY_PIXEL);
    slotRootParameter[1].InitAsConstantBufferView(0);
    slotRootParameter[2].InitAsConstantBufferView(1);
    slotRootParameter[3].InitAsConstantBufferView(2);

    auto staticSamplers =
        My::MyGE::RsrcMngrDX12::Instance().GetStaticSamplers();

    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
        4, slotRootParameter, (UINT)staticSamplers.size(),
        staticSamplers.data(),
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    My::MyGE::RsrcMngrDX12::Instance().RegisterRootSignature(
        ID_RootSignature_defer_light, &rootSigDesc);
  }
}

void WorldApp::BuildDescriptorHeaps() {}

void WorldApp::BuildShadersAndInputLayout() {
  std::filesystem::path hlslScreenPath = "../assets/shaders/Screen.hlsl";
  std::filesystem::path shaderScreenPath = "../assets/shaders/Screen.shader";
  std::filesystem::path hlslGeomrtryPath = "../assets/shaders/Geometry.hlsl";
  std::filesystem::path shaderGeometryPath =
      "../assets/shaders/Geometry.shader";
  std::filesystem::path hlslDeferPath = "../assets/shaders/deferLighting.hlsl";
  std::filesystem::path shaderDeferPath =
      "../assets/shaders/deferLighting.shader";

  if (!std::filesystem::is_directory("../assets/shaders"))
    std::filesystem::create_directories("../assets/shaders");

  auto& assetMngr = My::MyGE::AssetMngr::Instance();
  auto hlslScreen = assetMngr.LoadAsset<My::MyGE::HLSLFile>(hlslScreenPath);
  auto hlslGeomrtry = assetMngr.LoadAsset<My::MyGE::HLSLFile>(hlslGeomrtryPath);
  auto hlslDefer = assetMngr.LoadAsset<My::MyGE::HLSLFile>(hlslDeferPath);

  screenShader = new My::MyGE::Shader;
  geomrtryShader = new My::MyGE::Shader;
  deferShader = new My::MyGE::Shader;

  screenShader->hlslFile = hlslScreen;
  geomrtryShader->hlslFile = hlslGeomrtry;
  deferShader->hlslFile = hlslDefer;

  screenShader->vertexName = "VS";
  geomrtryShader->vertexName = "VS";
  deferShader->vertexName = "VS";

  screenShader->fragmentName = "PS";
  geomrtryShader->fragmentName = "PS";
  deferShader->fragmentName = "PS";

  screenShader->targetName = "5_0";
  geomrtryShader->targetName = "5_0";
  deferShader->targetName = "5_0";

  screenShader->shaderName = "Screen";
  geomrtryShader->shaderName = "Geometry";
  deferShader->shaderName = "Defer";

  if (!assetMngr.CreateAsset(screenShader, shaderScreenPath)) {
    delete screenShader;
    screenShader = assetMngr.LoadAsset<My::MyGE::Shader>(shaderScreenPath);
  }

  if (!assetMngr.CreateAsset(geomrtryShader, shaderGeometryPath)) {
    delete geomrtryShader;
    geomrtryShader = assetMngr.LoadAsset<My::MyGE::Shader>(shaderGeometryPath);
  }

  if (!assetMngr.CreateAsset(deferShader, shaderDeferPath)) {
    delete deferShader;
    deferShader = assetMngr.LoadAsset<My::MyGE::Shader>(shaderDeferPath);
  }

  My::MyGE::RsrcMngrDX12::Instance().RegisterShader(screenShader);
  My::MyGE::RsrcMngrDX12::Instance().RegisterShader(geomrtryShader);
  My::MyGE::RsrcMngrDX12::Instance().RegisterShader(deferShader);

  mInputLayout = {
      {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
       D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
      {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12,
       D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
      {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20,
       D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
  };
}

void WorldApp::BuildShapeGeometry() {
  auto mesh = My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Mesh>(
      "../assets/models/cube.obj");
  My::MyGE::RsrcMngrDX12::Instance().RegisterStaticMesh(
      My::MyGE::RsrcMngrDX12::Instance().GetUpload(), mesh);
  My::MyECS::ArchetypeFilter filter;
  filter.all = {My::MyECS::CmptType::Of<My::MyGE::MeshFilter>};
  auto meshFilters =
      world.entityMngr.GetCmptArray<My::MyGE::MeshFilter>(filter);
  for (auto meshFilter : meshFilters) meshFilter->mesh = mesh;
}

void WorldApp::BuildPSOs() {
  auto screenPsoDesc = My::MyDX12::Desc::PSO::Basic(
      My::MyGE::RsrcMngrDX12::Instance().GetRootSignature(
          ID_RootSignature_screen),
      nullptr, 0,
      My::MyGE::RsrcMngrDX12::Instance().GetShaderByteCode_vs(screenShader),
      My::MyGE::RsrcMngrDX12::Instance().GetShaderByteCode_ps(screenShader),
      mBackBufferFormat, DXGI_FORMAT_UNKNOWN);
  screenPsoDesc.RasterizerState.FrontCounterClockwise = TRUE;
  My::MyGE::RsrcMngrDX12::Instance().RegisterPSO(ID_PSO_screen, &screenPsoDesc);

  auto geometryPsoDesc = My::MyDX12::Desc::PSO::MRT(
      My::MyGE::RsrcMngrDX12::Instance().GetRootSignature(
          ID_RootSignature_geometry),
      mInputLayout.data(), (UINT)mInputLayout.size(),
      My::MyGE::RsrcMngrDX12::Instance().GetShaderByteCode_vs(geomrtryShader),
      My::MyGE::RsrcMngrDX12::Instance().GetShaderByteCode_ps(geomrtryShader),
      3, DXGI_FORMAT_R32G32B32A32_FLOAT, mDepthStencilFormat);
  geometryPsoDesc.RasterizerState.FrontCounterClockwise = TRUE;
  My::MyGE::RsrcMngrDX12::Instance().RegisterPSO(ID_PSO_geometry,
                                                 &geometryPsoDesc);

  auto deferLightingPsoDesc = My::MyDX12::Desc::PSO::Basic(
      My::MyGE::RsrcMngrDX12::Instance().GetRootSignature(
          ID_RootSignature_defer_light),
      nullptr, 0,
      My::MyGE::RsrcMngrDX12::Instance().GetShaderByteCode_vs(deferShader),
      My::MyGE::RsrcMngrDX12::Instance().GetShaderByteCode_ps(deferShader),
      mBackBufferFormat, DXGI_FORMAT_UNKNOWN);
  deferLightingPsoDesc.RasterizerState.FrontCounterClockwise = TRUE;
  My::MyGE::RsrcMngrDX12::Instance().RegisterPSO(ID_PSO_defer_light,
                                                 &deferLightingPsoDesc);
}

void WorldApp::BuildFrameResources() {
  for (const auto& fr : frameRsrcMngr->GetFrameResources()) {
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator;
    ThrowIfFailed(uDevice->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator)));

    fr->RegisterResource("CommandAllocator", allocator);

    fr->RegisterResource("gbPass constants",
                         My::MyDX12::ArrayUploadBuffer<PassConstants>{
                             uDevice.raw.Get(), 1, true});

    /*fr->RegisterResource("ArrayUploadBuffer<MaterialConstants>",
            My::MyDX12::ArrayUploadBuffer<MaterialConstants>{
    uDevice.raw.Get(), mMaterials.size(), true });

    fr->RegisterResource("ArrayUploadBuffer<ObjectConstants>",
            My::MyDX12::ArrayUploadBuffer<ObjectConstants>{ uDevice.raw.Get(),
    mAllRitems.size(), true });*/

    fr->RegisterResource("ShaderCBMngrDX12",
                         My::MyGE::ShaderCBMngrDX12{uDevice.raw.Get()});

    auto fgRsrcMngr = std::make_shared<My::MyDX12::FG::RsrcMngr>();
    fgRsrcMngr->Init(uGCmdList, uDevice);
    fr->RegisterResource("FrameGraphRsrcMngr", fgRsrcMngr);
  }
}

void WorldApp::BuildMaterials() {
  std::filesystem::path matPath = "../assets/materials/iron.mat";
  auto material = new My::MyGE::Material;
  material->shader = geomrtryShader;
  material->texture2Ds.emplace("gAlbedoMap", albedoTex2D);
  material->texture2Ds.emplace("gRoughnessMap", roughnessTex2D);
  material->texture2Ds.emplace("gMetalnessMap", metalnessTex2D);

  if (!My::MyGE::AssetMngr::Instance().CreateAsset(material, matPath)) {
    delete material;
    material =
        My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Material>(matPath);
  }
  My::MyECS::ArchetypeFilter filter;
  filter.all = {My::MyECS::CmptType::Of<My::MyGE::MeshRenderer>};
  auto meshRenderers =
      world.entityMngr.GetCmptArray<My::MyGE::MeshRenderer>(filter);
  for (auto meshRenderer : meshRenderers)
    meshRenderer->material.push_back(material);

  auto iron = std::make_unique<Material>();
  iron->Name = "iron";
  iron->MatCBIndex = 0;
  iron->AlbedoSrvGpuHandle =
      My::MyGE::RsrcMngrDX12::Instance().GetTexture2DSrvGpuHandle(albedoTex2D);
  iron->RoughnessSrvGpuHandle =
      My::MyGE::RsrcMngrDX12::Instance().GetTexture2DSrvGpuHandle(
          roughnessTex2D);
  iron->MetalnessSrvGpuHandle =
      My::MyGE::RsrcMngrDX12::Instance().GetTexture2DSrvGpuHandle(
          metalnessTex2D);
  iron->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
  iron->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
  iron->Roughness = 0.2f;

  mMaterials["iron"] = std::move(iron);
}

void WorldApp::BuildRenderItems() {
  // auto boxRitem = std::make_unique<RenderItem>();
  // boxRitem->ObjCBIndex = 0;
  // boxRitem->Mat = mMaterials["iron"].get();
  // boxRitem->Geo =
  // &My::MyGE::RsrcMngrDX12::Instance().GetMeshGPUBuffer(mesh);
  // boxRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
  // boxRitem->IndexCount = boxRitem->Geo->IndexBufferByteSize /
  // (boxRitem->Geo->IndexFormat == DXGI_FORMAT_R16_UINT ? 2 : 4);
  // boxRitem->StartIndexLocation = 0;
  // boxRitem->BaseVertexLocation = 0;
  // mAllRitems.push_back(std::move(boxRitem));

  //// All the render items are opaque.
  // for(auto& e : mAllRitems)
  //	mOpaqueRitems.push_back(e.get());
}

void WorldApp::DrawRenderItems(ID3D12GraphicsCommandList* cmdList) {
  UINT passCBByteSize =
      My::MyDX12::Util::CalcConstantBufferByteSize(sizeof(PassConstants));
  UINT matCBByteSize =
      My::MyDX12::Util::CalcConstantBufferByteSize(sizeof(MaterialConstants));
  UINT objCBByteSize =
      My::MyDX12::Util::CalcConstantBufferByteSize(sizeof(ObjectConstants));

  /*auto objectCB = frameRsrcMngr->GetCurrentFrameResource()
          ->GetResource<My::MyDX12::ArrayUploadBuffer<ObjectConstants>>("ArrayUploadBuffer<ObjectConstants>")
          .GetResource();
  auto matCB = frameRsrcMngr->GetCurrentFrameResource()
          ->GetResource<My::MyDX12::ArrayUploadBuffer<MaterialConstants>>("ArrayUploadBuffer<MaterialConstants>")
          .GetResource();

  // For each render item...
  for (size_t i = 0; i < ritems.size(); ++i)
  {
          auto ri = ritems[i];

          cmdList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
          cmdList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
          cmdList->IASetPrimitiveTopology(ri->PrimitiveType);

          D3D12_GPU_VIRTUAL_ADDRESS objCBAddress =
  objectCB->GetGPUVirtualAddress() + ri->ObjCBIndex * objCBByteSize;
          D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress()
  + ri->Mat->MatCBIndex * matCBByteSize;

          cmdList->SetGraphicsRootDescriptorTable(0,
  ri->Mat->AlbedoSrvGpuHandle); cmdList->SetGraphicsRootDescriptorTable(1,
  ri->Mat->RoughnessSrvGpuHandle); cmdList->SetGraphicsRootDescriptorTable(2,
  ri->Mat->MetalnessSrvGpuHandle); cmdList->SetGraphicsRootConstantBufferView(3,
  objCBAddress); cmdList->SetGraphicsRootConstantBufferView(5, matCBAddress);

          cmdList->DrawIndexedInstanced(ri->IndexCount, 1,
  ri->StartIndexLocation, ri->BaseVertexLocation, 0);
  }
  */

  auto& shaderCBMngr =
      frameRsrcMngr->GetCurrentFrameResource()
          ->GetResource<My::MyGE::ShaderCBMngrDX12>("ShaderCBMngrDX12");

  auto buffer = shaderCBMngr.GetBuffer(geomrtryShader);

  const auto& mat2objects =
      renderContext.objectMap.find(geomrtryShader)->second;

  size_t offset = passCBByteSize;
  for (const auto& [mat, objects] : mat2objects) {
    // For each render item...
    size_t objIdx = 0;
    for (size_t i = 0; i < objects.size(); i++) {
      auto object = objects[i];
      auto meshFilter =
          world.entityMngr.Get<My::MyGE::MeshFilter>(object.entity);
      auto mesh = meshFilter->mesh;
      auto& meshGPUBuffer =
          My::MyGE::RsrcMngrDX12::Instance().GetMeshGPUBuffer(mesh);
      const auto& submesh = mesh->GetSubMeshes().at(object.submeshIdx);
      cmdList->IASetVertexBuffers(0, 1, &meshGPUBuffer.VertexBufferView());
      cmdList->IASetIndexBuffer(&meshGPUBuffer.IndexBufferView());
      // submesh.topology
      cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

      D3D12_GPU_VIRTUAL_ADDRESS objCBAddress =
          buffer->GetResource()->GetGPUVirtualAddress() + offset +
          matCBByteSize + objIdx * objCBByteSize;
      D3D12_GPU_VIRTUAL_ADDRESS matCBAddress =
          buffer->GetResource()->GetGPUVirtualAddress() + offset;

      auto albedo = mat->texture2Ds.find("gAlbedoMap")->second;
      auto roughness = mat->texture2Ds.find("gRoughnessMap")->second;
      auto metalness = mat->texture2Ds.find("gMetalnessMap")->second;
      auto albedoHandle =
          My::MyGE::RsrcMngrDX12::Instance().GetTexture2DSrvGpuHandle(albedo);
      auto roughnessHandle =
          My::MyGE::RsrcMngrDX12::Instance().GetTexture2DSrvGpuHandle(
              roughness);
      auto matalnessHandle =
          My::MyGE::RsrcMngrDX12::Instance().GetTexture2DSrvGpuHandle(
              metalness);
      cmdList->SetGraphicsRootDescriptorTable(0, albedoHandle);
      cmdList->SetGraphicsRootDescriptorTable(1, roughnessHandle);
      cmdList->SetGraphicsRootDescriptorTable(2, matalnessHandle);
      cmdList->SetGraphicsRootConstantBufferView(3, objCBAddress);
      cmdList->SetGraphicsRootConstantBufferView(5, matCBAddress);

      cmdList->DrawIndexedInstanced(submesh.indexCount, 1, submesh.indexStart,
                                    submesh.baseVertex, 0);
      objIdx++;
    }
    offset += matCBByteSize + objects.size() * objCBByteSize;
  }
}
